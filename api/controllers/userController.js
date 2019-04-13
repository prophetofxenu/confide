var User = require('../models/user');

var async = require('async');

function userExists(username, email, callback) {
    
    console.log('userExists');
    
    async.parallel({
        usernameExists: function(callback) {
            User.find({ username: username }).exec(callback);
        },
        emailExists: function(callback) {
            User.find({ email: email }).exec(callback);
        }
    }, function(err, results) {
        if (err) { return next(err); }

        console.log('running callback');

        callback({
            usernameExists: results.usernameExists.length > 0,
            emailExists: results.emailExists.length > 0    
        });

    })

}

exports.newUser = function(req, res, next) {

    console.log('new user');

    var username = req.body.username;
    var email = req.body.email;

    console.log(username, email);

    userExists(username, email, function(result) {

        if (result.usernameExists && result.emailExists) {
            res.json({
                result: -1,
                message: "User with that username and email already exists"
            });
        }
        if (result.usernameExists) {
            res.json({
                result: -2,
                message: "User with that username already exists"
            });
        }
        if (result.emailExists) {
            res.json({
                result: -3,
                message: "User with that email already exists"
            });
        }

        console.log('send result');
        res.json({
            result: 0
        });
        console.log('sent result');
    })

}