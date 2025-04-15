from flask import Flask, request, jsonify
from flask_cors import CORS  
import re

app = Flask(__name__)
CORS(app)  

token_specs = [
    ('KEYWORD', r'\b(int|float|if|else|while|for|return|void|char|double|long|short|switch|case|break|continue|default)\b'),
    ('IDENTIFIER', r'\b[a-zA-Z_][a-zA-Z_0-9]*\b'),
    ('NUMBER', r'\b\d+(\.\d+)?\b'),
    ('STRING', r'"[^"]*"'),
    ('CHAR', r"'.'"),
    ('OPERATOR', r'[+\-*/%=&|<>!]=?|==|!=|\+\+|--'),
    ('SEPARATOR', r'[()[\]{};,]'),
    ('WHITESPACE', r'[ \t]+'),
    ('NEWLINE', r'\n'),
    ('MISMATCH', r'.'),
]

token_regex = '|'.join(f'(?P<{name}>{pattern})' for name, pattern in token_specs)
compiled_regex = re.compile(token_regex)

def tokenize(code):
    tokens = []
    errors = []
    line_num = 1

    for match in compiled_regex.finditer(code):
        kind = match.lastgroup
        value = match.group()

        if kind == 'NEWLINE':
            line_num += 1
        elif kind == 'WHITESPACE':
            continue
        elif kind == 'MISMATCH':
            errors.append(f'Unexpected token {value!r} at line {line_num}')
        else:
            tokens.append((kind, value, line_num))

    return tokens, errors

@app.route("/analyze", methods=["POST"])
def analyze():
    code = request.form.get("code", "")
    tokens, errors = tokenize(code)
    return jsonify({"tokens": tokens, "errors": errors})

if __name__ == "__main__":
    app.run(debug=True)
