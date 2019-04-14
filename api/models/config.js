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

ConfigSchema
.virtual('created_formatted')
.get(function() {
    return moment(this.created).format('MM-DD-YYYY');
});

ConfigSchema
.virtual('modified_formatted')
.get(function() {
    return moment(this.modified).format('MM-DD-YYYY');
});

module.exports = mongoose.model('Config', ConfigSchema);