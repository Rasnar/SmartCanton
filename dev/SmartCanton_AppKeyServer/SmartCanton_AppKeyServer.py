import uuid
from datetime import timedelta

from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, jwt_required, create_access_token, get_jwt_identity
from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(__name__)

app.config['SECRET_KEY'] = 'thisissecret'
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:////Users/David/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/todo.db'
#app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///E:/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/todo.db'
db = SQLAlchemy(app)

app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(days=30)
app.config['JWT_HEADER_TYPE'] = "JWT"


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


# Provide a method to create access tokens. The create_access_token()
# function is used to actually generate the token, and you can return
# it to the caller however you choose.
@app.route('/auth', methods=['POST'])
def login():
    if not request.is_json:
        return jsonify({"msg": "Missing JSON in request"}), 400

    username = request.json.get('username', None)
    password = request.json.get('password', None)

    if not username:
        return jsonify({"msg": "Missing username parameter"}), 400
    if not password:
        return jsonify({"msg": "Missing password parameter"}), 400

    user = User.query.filter_by(name=username).first()

    # If user exist and password correct
    if (user is None) or (not check_password_hash(user.password, password)):
        # if username != 'test' or password != 'test':
        return jsonify({"msg": "Bad username or password"}), 401

    # Identity can be any data that is json serializable
    access_token = create_access_token(identity=user.public_id)
    return jsonify(access_token=access_token), 200


def user_from_public_id(public_id):
    try:
        current_identity = User.query.filter_by(public_id=public_id).first()
        return current_identity
    except:
        return User.query.filter_by(public_id).first()


jwt = JWTManager(app)


@app.route('/user', methods=['GET'])
@jwt_required
def get_all_users():
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'msg': 'Cannot perform that function with your rights!'})

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
@jwt_required
def get_one_user(public_id):
    if not get_jwt_identity().admin:
        return jsonify({'msg': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'msg': 'No user found!'})

    user_data = {}
    user_data['public_id'] = user.public_id
    user_data['username'] = user.name
    user_data['password'] = user.password
    user_data['admin'] = user.admin

    return jsonify({'user': user_data})


@app.route('/user', methods=['POST'])
@jwt_required
def create_user():
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'msg': 'Cannot perform that function!'})

    data = request.get_json()

    hashed_password = generate_password_hash(data['password'], method='sha256')

    new_user = User(public_id=str(uuid.uuid4()), name=data['username'], password=hashed_password, admin=False)
    db.session.add(new_user)
    db.session.commit()

    return jsonify({'msg': 'New user created!'})


@app.route('/user/<public_id>', methods=['PUT'])
@jwt_required
def promote_user(public_id):
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'msg': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'msg': 'No user found!'})

    user.admin = True
    db.session.commit()

    return jsonify({'msg': 'The user has been promoted!'})


@app.route('/user/<public_id>', methods=['DELETE'])
@jwt_required
def delete_user(public_id):
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'msg': 'Cannot perform that function!'})

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'msg': 'No user found!'})

    db.session.delete(user)
    db.session.commit()

    return jsonify({'msg': 'The user has been deleted!'})


@app.route('/todo', methods=['GET'])
@jwt_required
def get_all_todos():
    todos = Todo.query.filter_by(user_id=user_from_public_id(get_jwt_identity()).id).all()

    output = []

    for todo in todos:
        todo_data = {}
        todo_data['id'] = todo.id
        todo_data['text'] = todo.text
        todo_data['complete'] = todo.complete
        output.append(todo_data)

    return jsonify({'todos': output})


@app.route('/todo/<todo_id>', methods=['GET'])
@jwt_required
def get_one_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=user_from_public_id(get_jwt_identity()).id).first()

    if not todo:
        return jsonify({'msg': 'No todo found!'})

    todo_data = {}
    todo_data['id'] = todo.id
    todo_data['text'] = todo.text
    todo_data['complete'] = todo.complete

    return jsonify(todo_data)


@app.route('/todo', methods=['POST'])
@jwt_required
def create_todo():
    data = request.get_json()

    new_todo = Todo(text=data['text'], complete=False, user_id=user_from_public_id(get_jwt_identity()).id)
    db.session.add(new_todo)
    db.session.commit()

    return jsonify({'msg': "Todo created!"})


@app.route('/todo/<todo_id>', methods=['PUT'])
@jwt_required
def complete_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=user_from_public_id(get_jwt_identity()).id).first()

    if not todo:
        return jsonify({'msg': 'No todo found!'})

    todo.complete = True
    db.session.commit()

    return jsonify({'msg': 'Todo item has been completed!'})


@app.route('/todo/<todo_id>', methods=['DELETE'])
@jwt_required
def delete_todo(todo_id):
    todo = Todo.query.filter_by(id=todo_id, user_id=user_from_public_id(get_jwt_identity()).id).first()

    if not todo:
        return jsonify({'msg': 'No todo found!'})

    db.session.delete(todo)
    db.session.commit()

    return jsonify({'msg': 'Todo item deleted!'})


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
