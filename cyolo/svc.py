from flask import Flask, request, make_response
import argparse
from SimpleBackend import SimpleBackend
from HeapBackend import HeapBackend

app = Flask(__name__)
backend = None

def parse_cmd_line():
    usage = '''cyolo exercise'''
    parse = argparse.ArgumentParser(description=usage)

    parse.add_argument('--port', '-p', type=int, default=8000, dest='port', \
                       help='listening port')

    parse.add_argument('--host', '-i', type=str, dest='host', default='localhost', \
                       help='listening on host')

    parse.add_argument('--debug', '-d', type=bool, default=False, dest='debug', \
                       help='debug on')

    parse.add_argument('--backend', '-b', type=str, dest='backend', default='simple', \
                       help='type of backend (simple|steady)')



    return parse.parse_args()

@app.route('/exercise/add-text', methods=['POST'])
def add_text():
    text = request.data
    backend.add_text(str(text))
    return "text received"

@app.route('/exercise/stats')
def get_statistics():
    stats = backend.get_stats()

    resp = make_response(stats)
    resp.mimetype = "application/text"
    return resp

def main():
    global backend

    cmd = parse_cmd_line()
    if cmd.backend == "simple":
        backend = SimpleBackend()
    elif cmd.backend == "heap":
        backend = HeapBackend()
    else:
        print(f"Error: backend type {cmd.backend} not supported")
        exit(1)

    app.run(debug=cmd.debug, port=cmd.port, host=cmd.host)


if __name__ == '__main__':
    main()
