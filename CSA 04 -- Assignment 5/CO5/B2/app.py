from flask import Flask, jsonify
app = Flask(__name__)

@app.route("/")
def index():
    return "Campus Result Portal - results are available after login."

@app.route("/health")
def health():
    return jsonify(status="ok")
