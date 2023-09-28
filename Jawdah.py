## Last update: March 28, 2022 

########################################################## Importing libraries ##################################################
import requests
import spacy
import sys
import pandas as pd
from itertools import chain
from spiral import ronin
import nltk
import ssl

try:
    _create_unverified_https_context = ssl._create_unverified_context
except AttributeError:
    pass
else:
    ssl._create_default_https_context = _create_unverified_https_context

nltk.download("wordnet")

from nltk.corpus import wordnet
# https://spacy.io/usage/linguistic-features
nlp = spacy.load("en_core_web_sm")
#################################################################################################################################

####################################################### Define global variables #################################################
input_file_path = sys.argv[1] # Change this line to the name of the file that containes the method names. 
output_file_path = "scoreReport.csv" # Name the output file that will contains the score & feedback as you want. 
label_file_path = "labelsReport.csv" # Name the output file that will contains the labels & feedback as you want. 

# Non-Dictionary words (WordNet does not have them), but we are considering them as dictionary words
new_words = ["url", "to", "html", "eol","utf8","unicode","runtime" ,"iterator","utf16", "sync" ,"matlab","with", "this","main", "delim","popup","init","deinit", "misc", "string", "json", "from", "attribut", "of","lexer","delimiter","delimiters","printf", "meta", "multi", "verilog", "api", "keywords", "prev", "reconfigure", "func","cmp","calc","rect", "info", "dev", "if", "config","param", "ascii", "for", "should", "ctrls", "invisibles", "expr", "mssql", "oscript", "metapost","into","scintilla", "recusively", "plugin","and", "uri","unset","int", "whitespace", "gif" , "dijkstra" , "jpeg" , "sqr" , "bmp" , "png" , "object3d", "iphone", "cannot", "xml", "else", "preprocessor", "buildable", "namespace", "sharepoint", "newline", "unindent", "bindable"] # This is the list the evluators agree they should be dictionary terms

# Method names should not contain single letters and words such as string,str,integer,int,character, gimp
not_included_words = ['gimp']

# if a method name starts with any of these word spot them and if word is there BUT NOT in starting, then consider that name
special_words = ["scintilla", "should", "could", "can", "m"]

# This is a list of exception words and it is used for following the naming style rule
exception_list = ["HTML", "URL", "ID","UTF8","UTF16","EOL","Api","ASCII", "URI", "OScript", "METAPOST", "IsA", "isA", "3d" ,"LUT256", "IPhone", "XML", "Cannot"] # This is the list of the known abbreviations and acronyms by the five evaluators.
# Dictionary of abbreviation as key and value as its full form: keep them in lower-case. Add more if found out.
# for handling abbreviation with the tagger
abbs_full_form = {'id': 'indentity', 'sync':'synchronize' , 'doc':'documentation', 'delim':'delimiter', 'init':'initialize','deinit':'deinitialize', 'misc':'miscellaneous','str':'string', 'bidi':'bidirectional', 'prev':'previous', 'func':'function', 'cmp':'compare', 'calc':'calculate', 'rect':'rectangle', 'sci':'Scintilla', 'info':'information', 'dev':'device', 'config':'configuration','param':'parameter','Py':'python', 'Ecl': 'E-Code Language', 'ctrls':'controls', 'expr': 'expression' , 'sqr':'Square Root'}

# list of prepositions (to check the grammatical rule); Add or remove if find more
prepositions = ['aboard', 'about', 'above', 'across', 'after', 'against', 'along', 'amid', 'among', 'anti', 'around', 'at',
                'before', 'behind', 'below', 'beneath', 'beside', 'besides', 'between', 'beyond', 'but', 'by', 'despite', 'down',
                'during', 'for', 'from', 'in', 'inside', 'into', 'minus', 'near', 'of', 'off', 'on', 'onto',  'outside', 'over',
                'per','than', 'through', 'to', 'toward', 'towards', 'under', 'underneath', 'until', 'up', 'upon', 'versus', 'via',
                'with', 'within', 'without', "this", "that", "do", "if"]
#################################################################################################################################


####################################################### Defining functions #######################################################
def load_file(path):

    df = pd.read_csv(path)
    return df

def follow_underscore(text):

    # Rule-1
    if ((text.startswith("_")) or (text.endswith('_')) or ("_" not in text)): return False
    # Rule-2:
    if "-" in text: return False # for text like "read_book-ok"

    # Rule-3 & Rule-4
    continuous_underscrore_count = 0
    for letter in text[1:]:
        if letter.isupper(): return False #Rule-3

        if letter.isdigit(): continue #letter.islower() or
        if letter == "_":continuous_underscrore_count+=1
        else: continuous_underscrore_count = 0
        if continuous_underscrore_count>=2: return False # Rule-4
    return True

def follow_KebabCase(text):

    # Rule-1
    if ((text.startswith("-")) or (text.endswith('-')) or ("-" not in text)): return False
    # Rule-2:
    if "_" in text: return False # for text like "read_book-ok"

    # Rule-3 & Rule-4
    continuous_dash_count = 0
    for letter in text[1:]:
        if letter.isupper(): return False #Rule-3

        if letter.isdigit(): continue #letter.islower() or
        if letter == "-":continuous_dash_count+=1
        else: continuous_dash_count = 0
        if continuous_dash_count>=2: return False # Rule-4
    return True

def follow_camelCase(text):

    if (('_' in text) or ('-' in text)): return False # Rule-1
    if not text[0].islower(): return False # Rule-2

    # Rule-3 and Rule-4
    is_upper = False
    continuous_upper_letter_count = 0
    for letter in text[1:]:
        if letter.isupper():
            continuous_upper_letter_count += 1
            is_upper = True # a variable for: Rule-3
        else:continuous_upper_letter_count=0
        if continuous_upper_letter_count>=2: return False #Rule-4
    if is_upper: return True # Rule-3
    else: return False

def follow_PascalCase(text):

    if (('_' in text) or ('-' in text)): return False # Rule-1
    if not text[0].isupper(): return False # Rule-2

    # Rule-3 and Rule-4
    is_lower = False
    continuous_upper_letter_count = 0
    for letter in text[1:]:
        if letter.isupper():continuous_upper_letter_count += 1
        else:continuous_upper_letter_count=0; is_lower = True
        if continuous_upper_letter_count>=2: return False #Rule-4
    if is_lower: return True # Rule-3
    else: return False

def handle_exception(text, exp_list):

    if text.startswith("_") or text.startswith("-") or text.endswith("_") or text.endswith("-"): return False
    for word in exp_list:
        if word in text: return True
    else: return False


def follow_naming_rule(text, exp_list, score = 1):

    report = {}
    splited_phrase = ronin.split(text) # this is a list which contains words after spliting the name
    if len(splited_phrase)>1:
        if (follow_underscore(text) or follow_KebabCase(text) or follow_camelCase(text)
            or follow_PascalCase(text) or handle_exception(text, exp_list)):
            report["score"] = score
            report["comment"] = None
            report["phrase"] = text
        else: report["score"] = 0; report["comment"] = 'Follow a naming style'; report["phrase"] = text
    elif len(splited_phrase)==1:
        report["score"] = score
        report["comment"] = None
        report["phrase"] = text
    else: # i.e. phrase has no letters in it (len(splited_phrase)==0)
        report["score"] = 0
        report["comment"] = 'Add at least one word or a letter to check the naming style standard'
        report["phrase"] = text

    return report

def wordnet_or_exception_word_rule(name_rule_report, new_list, score = 3):

    phrase = name_rule_report.get("phrase")
    report = {}
    if name_rule_report.get("score"):
        # split the phrase
        if follow_underscore(phrase):
            phrase = " ".join(phrase.split("_"))
        elif follow_KebabCase(phrase):
            phrase = " ".join(phrase.split("_"))
        else: 
            phrase = " ".join(ronin.split(phrase))

        report["phrase"] = phrase

        # checking in WordNet and for some special words
        for word in phrase.split(" "):
            if (len(wordnet.synsets(word))==0) and (word.lower() not in new_list):
                report["score"] = 0
                report["comment"] = "Use correct dictionary words"
                return report
        else:
            report["score"] = score
            report["comment"] = None
            return report
    else:
        report["score"] = 0
        report["comment"] = "Use correct dictionary words"
        report["phrase"] = phrase
        return report

def grammer_rule_func(wordnet_rule, prepositions, score=1):

    report = dict()
    phrase = wordnet_rule.get("phrase")
    report['phrase'] = phrase

    splitted_phrase = ronin.split(phrase)

    if len(splitted_phrase)>0:
        if ((splitted_phrase[0].lower() in prepositions) or (splitted_phrase[-1].lower() in prepositions)):
            report["score"] = 0
            report["comment"] = "Check the grammatical structure of the method name; the name starts or ends with preposition"
        else:
            report["score"] = 1
            report["comment"] = None
    else:
        report["score"] = 0
        report["comment"] = "Can't check for the grammar structure standard because no word or letter in the name"
    return report

def phrase_verb_rule(wordnet_rule, abbrevation_hastable, text, identifier_type, identifier_context, score=1, use_api=False):

    report = {}
    temp = []
    if use_api:
        identifier_name = text
        identifier_name += '()'
        
        # use the api to predict the tags
        req = requests.get(f'http://127.0.0.1:5000/{identifier_type}/{identifier_name}/{identifier_context}')
        pred = req.text.split(",")
        
        for word in pred:
            if word.split("|")[-1].strip().lower() == 'v':
                report["score"] = score
                report["comment"] = None
                break
        else:
            report["score"] = 0; report["comment"] = "Add a verb to the method name"
        report["phrase"] = wordnet_rule.get("phrase")
    else:
        phrase = wordnet_rule.get("phrase")
        for word in phrase.split(" "): # replacing the abbreviation
            if abbrevation_hastable.get(word.lower()): temp.append(abbrevation_hastable.get(word.lower()))
            else: temp.append(word)
        phrase = " ".join(temp)
        # check the VERB condition
        doc = nlp(phrase)
        for token in doc:
            if token.pos_ == "VERB": report["score"] = score; report["comment"] = None; break
        else: report["score"] = 0; report["comment"] = "Add a verb to the method name"
        report["phrase"] = phrase
    return report

def single_letter_rule(verb_rule, score=1):

    report = {}
    for word in verb_rule.get("phrase").split(" "):
        if len(word) == 1: report["score"] = 0;report["comment"] = f"Name should not contain `{word}` as it is single letter"; break
    else: report["score"] = 1; report["comment"] = ''
    report["phrase"] = verb_rule.get('phrase')
    return report

def phrase_starts_with_special_words_rule(single_letter_rule, special_list, score=1):

    phrase = single_letter_rule.get('phrase')
    report = {}
    for word in special_list:
        if phrase.split()[0].lower() == word:
            report["score"] = 0; report["comment"] = f"Method name should not start with `{phrase.split()[0]}`"; break
    else: report["score"] = score; report["comment"] = None
    report["phrase"] = phrase
    return report

def phrase_word_count_rule(starts_with_special_rule, score = 1):

    report = {}
    if len(starts_with_special_rule.get("phrase").split(' '))<7:report["score"]=score; report["comment"]=None
    else:report["score"] = 0;report["comment"] = "Number of words must be less than 7"
    report["phrase"] = starts_with_special_rule.get("phrase")
    return report

def contain_specail_word_rule(word_count_rule, not_included_words, score = 1):

    report = {}
    for word in word_count_rule.get("phrase").split(" "):
        if word.lower() in not_included_words: report["score"] = 0; report["comment"] = f"contains special word `{word}`"; break
    else: report["score"] = score; report["cooment"] = None
    report["phrase"] = word_count_rule.get("phrase")
    return report


def phrase_score(text, identifier_type, identifier_context):

    # rule:1
    name_rule = follow_naming_rule(text, exception_list)
    # rule:2
    wordnet_rule = wordnet_or_exception_word_rule(name_rule, new_words)
    # rule: grammer-rule
    grammer_rule = grammer_rule_func(wordnet_rule, prepositions)
    # rule:3
    verb_rule = phrase_verb_rule(wordnet_rule, abbs_full_form, text, identifier_type, identifier_context)
    # rule:4
    letter_rule = single_letter_rule(verb_rule)
    # rule:5
    start_word_rule = phrase_starts_with_special_words_rule(letter_rule, special_words)
    # rule:6
    word_count_rule = phrase_word_count_rule(start_word_rule)
    # rule:7
    specail_word_rule = contain_specail_word_rule(word_count_rule, not_included_words)

    report = {"score": 0, "comment":''}
    comments = []
    for temp in [name_rule, wordnet_rule, grammer_rule, verb_rule, letter_rule, start_word_rule, word_count_rule, specail_word_rule]:
        report["score"] += temp.get("score")
        if temp.get("comment"):comments.append(temp.get("comment"))
    report["comment"] = "/".join(comments)
    return report

def lable_the_scre(score):
    """
    ls: list of scores
        return the list of labels
    """
    if score>=0 and score<=1: return "Bad method name"
    elif score<=5:  return "Poor method name"
    elif score<=8: return "Fair method name"
    elif score<=9: return "Good method name"
    elif score==10: return "Excellent method name"
    else: return "Invalid score"

def score(phrases_df):
    """
    ls: list of method names
        this will list of method names along with their scores and comments
    """
    new_ls = []
    label_scores = []
    for _, row in phrases_df.iterrows():
        identifier_type = row[1] # method context
        phrase = row[0] # method name
        identifier_context = row[1] # method context

        if phrase[-2:] == '()':
            phrase = phrase[:-2]
        temp = phrase+", "; temp1 = phrase+", "
        report = phrase_score(phrase, identifier_type, identifier_context)
        temp += f"{report.get('score')} out of 10"
        temp1 += lable_the_scre(report.get('score'))
        if report.get("comment"):
            temp += ", "; temp1 += ", "
            temp += report.get("comment"); temp1 += report.get("comment")
        new_ls.append(temp); label_scores.append(temp1)
    return new_ls, label_scores

########################################################################################################################

def main():
    phrases_df = load_file(input_file_path)
    output, label_scores = score(phrases_df)
    with open(output_file_path, "w") as f:
        f.write("Name,Score,Comments\n")
        f.write("\n".join(output))
    with open(label_file_path, "w") as f:
        f.write("Name, Lable, Comments\n")
        f.write("\n".join(label_scores))


if __name__ == "__main__":
    main()
