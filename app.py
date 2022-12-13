from flask import Flask, render_template
from ClientMQTT import ClientMQTT

app = Flask(__name__)
app.config['DEBUG'] = True
client = ClientMQTT()
client.run()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/receive', methods=['GET'])
def getLixeirasByNumber():
    try:
        
        return render_template('index.html')
    except Exception as ex:
        return f"Erro: {ex}"

@app.route('/publish', methods=['GET'])
def getLixeiraByID():
    try:
        return "321"
    except Exception as ex:
        return f"Erro: {ex}"


if __name__ == "__main__":
    app.run()