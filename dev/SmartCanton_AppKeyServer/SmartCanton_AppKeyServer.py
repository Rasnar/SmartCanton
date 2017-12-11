import uuid
from datetime import timedelta

from flask import Flask, request, jsonify
from flask_jwt_extended import JWTManager, jwt_required, create_access_token, get_jwt_identity
from flask_sqlalchemy import SQLAlchemy
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(__name__)

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:////Users/David/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/' \
                                        'smartcanton_users_devices.db'
# app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///E:/Dropbox/SmartCanton/dev/SmartCanton_AppKeyServer/' \
#                                         'smartcanton_users_devices.db'

app.config['SECRET_KEY'] = 'thisissecret'
db = SQLAlchemy(app)

app.config['JWT_ACCESS_TOKEN_EXPIRES'] = timedelta(days=30)
app.config['JWT_HEADER_TYPE'] = "JWT"
app.config['JWT_IDENTITY_CLAIM'] = "public_id"

jwt = JWTManager(app)


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    public_id = db.Column(db.String(50), unique=True)
    name = db.Column(db.String(50))
    password = db.Column(db.String(80))
    admin = db.Column(db.Boolean)


class LoraDevice(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_eui = db.Column(db.CHAR(16), unique=True)
    app_eui = db.Column(db.CHAR(16))
    app_key = db.Column(db.CHAR(32))
    owner_id = db.Column(db.Integer, db.ForeignKey('user.id'))
    owner = db.relationship('User', foreign_keys=owner_id)


# Provide a method to create access tokens. The create_access_token()
# function is used to actually generate the token, and you can return
# it to the caller however you choose.
@app.route('/auth', methods=['POST'])
def login():
    if not request.is_json:
        return jsonify({"message": "Missing JSON in request"}), 400

    try:
        username = request.json.get('username', None)
        password = request.json.get('password', None)
    except:
        username = None
        password = None

    if not username:
        return jsonify({"message": "Missing username parameter"}), 400
    if not password:
        return jsonify({"message": "Missing password parameter"}), 400

    user = User.query.filter_by(name=username).first()

    # If user exist and password correct
    if (user is None) or (not check_password_hash(user.password, password)):
        # if username != 'test' or password != 'test':
        return jsonify({"message": "Bad username or password"}), 401

    # Identity can be any data that is json serializable
    access_token = create_access_token(identity=user.public_id)
    return jsonify(access_token=access_token), 200


def user_from_public_id(public_id):
    try:
        current_identity = User.query.filter_by(public_id=public_id).first()
        return current_identity
    except:
        return User.query.filter_by(public_id).first()


@app.route('/user', methods=['GET'])
@jwt_required
def get_all_users():
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'message': 'Cannot perform that function with your rights!'}), 403

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
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'message': 'Cannot perform that function!'}), 403

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'}), 403

    user_data = {}
    user_data['public_id'] = user.public_id
    user_data['username'] = user.name
    # user_data['password'] = user.password
    user_data['admin'] = user.admin

    # return jsonify({'user': user_data})
    return jsonify(user_data)


@app.route('/user', methods=['POST'])
@jwt_required
def create_user():
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'message': 'Cannot perform that function!'}), 403

    data = request.get_json()

    hashed_password = generate_password_hash(data['password'], method='sha256')

    new_user = User(public_id=str(uuid.uuid4()), name=data['username'], password=hashed_password, admin=False)
    db.session.add(new_user)
    db.session.commit()

    return jsonify({'message': 'New user created!'})


@app.route('/user/<public_id>', methods=['PUT'])
@jwt_required
def promote_user(public_id):
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'message': 'Cannot perform that function!'}), 403

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'}), 404

    user.admin = True
    db.session.commit()

    return jsonify({'message': 'The user password has been changed!'})


@app.route('/user/<public_id>', methods=['DELETE'])
@jwt_required
def delete_user(public_id):
    if not user_from_public_id(get_jwt_identity()).admin:
        return jsonify({'message': 'Cannot perform that function!'}), 403

    user = User.query.filter_by(public_id=public_id).first()

    if not user:
        return jsonify({'message': 'No user found!'}), 400

    db.session.delete(user)
    db.session.commit()

    return jsonify({'message': 'The user has been deleted!'})


@app.route('/device', methods=['GET'])
@jwt_required
def get_all_devices_eui():
    if user_from_public_id(get_jwt_identity()).admin:
        devices = LoraDevice.query.all()
    else:
        devices = LoraDevice.query.filter_by(owner_id=user_from_public_id(get_jwt_identity())).all()

    output = []

    for device in devices:
        device_data = {}
        device_data['dev_eui'] = device.device_eui
        output.append(device_data)

    return jsonify({'devices': output})


@app.route('/device/<dev_eui>', methods=['GET'])
@jwt_required
def get_one_device_eui(dev_eui):
    if user_from_public_id(get_jwt_identity()).admin:
        device = LoraDevice.query.filter_by(device_eui=dev_eui).first()
    else:
        device = LoraDevice.query.filter_by(device_eui=dev_eui,
                                            owner_id=user_from_public_id(get_jwt_identity())).first()

    if not device:
        return jsonify({'message': 'No device found!'}), 400

    device_data = {}
    device_data['dev_eui'] = device.device_eui
    device_data['app_eui'] = device.app_eui
    device_data['app_key'] = device.app_key
    device_data['owner_public_id'] = User.query.filter_by(id=device.owner_id).first().public_id

    return jsonify(device_data)


@app.route('/device', methods=['POST'])
@jwt_required
def create_device():
    data = request.get_json()

    new_device = None

    try:
        new_device = LoraDevice(device_eui=data['dev_eui'],
                                app_eui=data['app_eui'],
                                app_key=data['app_key'],
                                owner_id=User.query.filter_by(public_id=data['owner_public_id']).first().id)
    except:
        return jsonify({'message': "New device could NOT be created!"}), 403

    try:
        db.session.add(new_device)
        db.session.commit()
    except:
        return jsonify({'message': "Device already present inside the database!"}), 403

    return jsonify({'message': "New device created!"})


@app.route('/device/<dev_eui>', methods=['PUT'])
@jwt_required
def update_device(dev_eui):
    data = request.get_json()

    if user_from_public_id(get_jwt_identity()).admin:
        device = LoraDevice.query.filter_by(device_eui=dev_eui).first()
    else:
        device = LoraDevice.query.filter_by(device_eui=dev_eui,
                                            owner_id=user_from_public_id(get_jwt_identity())).first()

    if not device:
        return jsonify({'message': 'Device not found!'}), 403

    try:
        device.device_eui = device['dev_eui']
        device.app_eui = device['app_eui']
        device.app_key = device['app_key']
        device.public_id = device['owner_public_id']
    except:
        return jsonify({'message': "Device parameter not valid!"}), 401

    try:
        db.session.commit()
    except:
        return jsonify({'message': "Device parameter not valid!"}), 401

    return jsonify({'message': "New device created!"})


@app.route('/device/<dev_eui>', methods=['DELETE'])
@jwt_required
def delete_device(dev_eui):
    if user_from_public_id(get_jwt_identity()).admin:
        device = LoraDevice.query.filter_by(device_eui=dev_eui).first()
    else:
        return jsonify({'message': 'Cannot perform that function!'}), 403

    if not device:
        return jsonify({'message': 'Device not found!'}), 403

    db.session.delete(device)
    db.session.commit()

    return jsonify({'message': 'Lora Device item deleted!'})


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
