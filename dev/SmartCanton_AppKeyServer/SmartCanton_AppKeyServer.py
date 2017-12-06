from flask import Flask, request, jsonify, make_response
from flask_sqlalchemy import SQLAlchemy
import uuid
from werkzeug.security import generate_password_hash, check_password_hash
from datetime import datetime, timedelta
# import jwt
from flask_jwt import JWT, jwt_required, current_identity
import datetime
from functools import wraps

app = Flask(__name__)

app.config['SECRET_KEY'] = 'thisissecret'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:////Users/David/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/todo.db'
#app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///E:/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/todo.db'
db = SQLAlchemy(app)

app.config['JWT_EXPIRATION_DELTA'] = timedelta(days=30)

class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    public_id = db.Column(db.String(50), unique=True)
    name = db.Column(db.String(50))
    password = db.Column(db.String(80))
    admin = db.Column(db.Boolean)


class Todo(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    text = db.Column(db.String(50))
    complete = db.Column(db.Boolean)
    user_id = db.Column(db.Integer)

def authenticate(username, password):
    user = User.query.filter_by(name=username).first()

    # If user exist and password correct
    if user and check_password_hash(user.password, password):
        return user

def identity(payload):
    public_id = payload['public_id']
    try:
        current_identity = User.query.filter_by(public_id=public_id).first()
        return current_identity
    except:
        return User.query.filter_by(public_id).first()


def make_jwt_payload(identity):
    iat = datetime.datetime.utcnow()
    exp = iat + app.config.get('JWT_EXPIRATION_DELTA')
    nbf = iat + app.config.get('JWT_NOT_BEFORE_DELTA')
    return {'exp': exp, 'iat': iat, 'nbf': nbf, 'public_id': identity.public_id}

jwt = JWT(app, authenticate, identity)
jwt.jwt_payload_handler(make_jwt_payload)

@app.route('/user', methods=['GET'])
@jwt_required()
def get_all_users():
    if not current_identity.admin:
        return jsonify({'message': 'Cannot perform that function!'})

    users = User.query.all()

    output = []

    for user in users:
        user_data = {}
        user_data['public_id'] = user.public_id
        user_data['name'] = user.name
        user_data['password'] = user.password
        user_data['admin'] = user.admin
        output.append(user_data)

    return jsonify({'users': output})


@app.route('/user/<public_id>', methods=['GET'])
@jwt_required()
def get_one_user(public_id):
    if not current_identity.admin:
        return jsonify({'message': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'})

    user_data = {}
    user_data['public_id'] = user.public_id
    user_data['username'] = user.name
    user_data['password'] = user.password
    user_data['admin'] = user.admin

    return jsonify({'user': user_data})


@app.route('/user', methods=['POST'])
@jwt_required()
def create_user():
    if not current_identity.admin:
        return jsonify({'message': 'Cannot perform that function!'})

    data = request.get_json()

    hashed_password = generate_password_hash(data['password'], method='sha256')

    new_user = User(public_id=str(uuid.uuid4()), name=data['username'], password=hashed_password, admin=False)
    db.session.add(new_user)
    db.session.commit()

    return jsonify({'message': 'New user created!'})


@app.route('/user/<public_id>', methods=['PUT'])
@jwt_required()
def promote_user(public_id):
    if not current_identity.admin:
        return jsonify({'message': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'})

    user.admin = True
    db.session.commit()

    return jsonify({'message': 'The user has been promoted!'})


@app.route('/user/<public_id>', methods=['DELETE'])
@jwt_required()
def delete_user(public_id):
    if not current_identity.admin:
        return jsonify({'message': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'})

    db.session.delete(user)
    db.session.commit()

    return jsonify({'message': 'The user has been deleted!'})


@app.route('/todo', methods=['GET'])
@jwt_required()
def get_all_todos():
    todos = Todo.query.filter_by(user_id=current_identity.id).all()

    output = []

    for todo in todos:
        todo_data = {}
        todo_data['id'] = todo.id
        todo_data['text'] = todo.text
        todo_data['complete'] = todo.complete
        output.append(todo_data)

    return jsonify({'todos': output})


@app.route('/todo/<todo_id>', methods=['GET'])
@jwt_required()
def get_one_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=current_identity.id).first()

    if not todo:
        return jsonify({'message': 'No todo found!'})

    todo_data = {}
    todo_data['id'] = todo.id
    todo_data['text'] = todo.text
    todo_data['complete'] = todo.complete

    return jsonify(todo_data)


@app.route('/todo', methods=['POST'])
@jwt_required()
def create_todo():
    data = request.get_json()

    new_todo = Todo(text=data['text'], complete=False, user_id=current_identity.id)
    db.session.add(new_todo)
    db.session.commit()

    return jsonify({'message': "Todo created!"})


@app.route('/todo/<todo_id>', methods=['PUT'])
@jwt_required()
def complete_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=current_identity.id).first()

    if not todo:
        return jsonify({'message': 'No todo found!'})

    todo.complete = True
    db.session.commit()

    return jsonify({'message': 'Todo item has been completed!'})


@app.route('/todo/<todo_id>', methods=['DELETE'])
@jwt_required()
def delete_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=current_identity.id).first()

    if not todo:
        return jsonify({'message': 'No todo found!'})

    db.session.delete(todo)
    db.session.commit()

    return jsonify({'message': 'Todo item deleted!'})



if __name__ == '__main__':
    app.run(debug=True)