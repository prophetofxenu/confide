var mongoose = require('mongoose');
var moment = require('moment');

var Schema = mongoose.Schema;

var ConfigSchema = new Schema(
    {
        name: { type: String, required: true },
        path: { type: String, required: true },
        content: { type: String, required: true },
        created: { type: Date, required: true },
        modified: { type: Date, required: true }
    }
)

module.exports = mongoose.model('Config', ConfigSchema);