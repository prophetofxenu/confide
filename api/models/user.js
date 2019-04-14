var mongoose = require('mongoose');
var moment = require('moment');

var Schema = mongoose.Schema;

var UserSchema = new Schema(
    {
        username: { type: String, required: true },
        password: { type: String, required: true },
        email: { type: String, required: true },
        created: { type: Date, required: true },
        lastlogin: { type: Date, required: true },
        configs: [{ type: Schema.Types.ObjectId, required: true, ref: 'Config'}]
    }
);

module.exports = mongoose.model('User', UserSchema);