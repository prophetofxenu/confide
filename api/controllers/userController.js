var Config = require('../models/config');
var User = require('../models/user');

var async = require('async');
var bcrypt = require('bcrypt');

function userExists(username, email, callback) {
    
    async.parallel({
        usernameExists: function(callback) {
            User.find({ username: username }).exec(callback);
        },
        emailExists: function(callback) {
            User.find({ email: email }).exec(callback);
        }
    }, function(err, results) {
        if (err) { return next(err); }

        callback({
            usernameExists: results.usernameExists.length > 0,
            emailExists: results.emailExists.length > 0    
        });

    })

}

function hashPasswordAndCreateUser(username, password, email) {

    bcrypt.hash(password, 10, createUser(username, email));

}

function createUser(username, email) {

    return function(err, hash) {
        if (err) { console.log(err); }

        var user = new User({
            username: username,
            password: hash,
            email: email,
            created: new Date(),
            lastlogin: new Date(),
            configs: []
        });

        user.save(function(err) {
            if (err) { console.log(err); }
        });
    }

}

exports.newUser = function(req, res, next) {

    var username = req.body.username;
    var email = req.body.email;

    userExists(username, email, function(result) {

        if (result.usernameExists && result.emailExists) {
            res.json({
                result: -1,
                message: "User with that username and email already exists"
            });
        } else if (result.usernameExists) {
            res.json({
                result: -2,
                message: "User with that username already exists"
            });
        } else if (result.emailExists) {
            res.json({
                result: -3,
                message: "User with that email already exists"
            });
        } else {

            hashPasswordAndCreateUser(username, req.body.password, email);

            res.json({
                result: 0
            });
        
        }

    })

}

function getUserConfs(username, password, callback) {

    User.find({username: username})
    .populate('configs')
    .exec(authenticateAndGetConfs(password, callback));

}

function authenticateAndGetConfs(password, callback) {

    return function(err, result) {
        if (err) { console.log(err); }

        if (result.length == 0) {
             callback(null, -1); // user not found
        } else {
            var user = result[0];
            bcrypt.compare(password, user.password, verifyAuthenticationAndGetConfs(user, callback))
        }
    }

}

function verifyAuthenticationAndGetConfs(user, callback) {

    return function(err, result) {
        if (err) { console.log(err); }

        if (!result) {
            callback(null, -2); // wrong password
        } else {
            callback(null, user.configs);
        }
    }

}

exports.listConfs = function(req, res, next) {

    var j = JSON.parse(req.params.json);

    getUserConfs(j.username, j.password, function(err, result) {
        if (err) { return next(err); }

        if (result == -1) { // user not found
            res.json({
                result: result,
                message: "User not found"
            });
        } else if (result == -2) {
            res.json({
                result: result,
                message: "Wrong password"
            });
        } else {
            var configNames = [];
            for (c of result) {
                configNames.push(c.name);
            }
            res.json({
                result: 0,
                configs: configNames
            });
        }
    });

}

function getUser(username, password, configName, callback) {

    User.find({
        username: username
    })
    .populate('configs')
    .exec(hashPasswordAndLogin(password, configName, callback));

}

function hashPasswordAndLogin(password, configName, callback) {

    return function(err, result) {

        if (err) { return next(err); }

        if (result.length == 0) {
            callback(null, -1); // user not found
            return;
        }

        var user = result[0];
        bcrypt.compare(password, user.password, function(err, res) {
            if (err) { return next(err); }

            if (res) {
                getConfig(user, configName, callback);
            } else {
                callback(null, -2); // wrong password
            }
        })

    }

}

function getConfig(user, configName, callback) {

    var config = null;
    for (c of user.configs) {
        if (c.name == configName) {
            config = c;
        }
    }
    if (config == null) {
        callback(null, -3); // config doesnt exist
    } else {
        callback(null, config);
    }

}

exports.getConf = function(req, res, next) {

    var j = JSON.parse(req.params.json);

    getUser(j.username, j.password, j.name, function(err, result) {

        if (err) { return next(err); }

        console.log('get user');

        if (result == -1) {
            res.json({
                result: result,
                message: "User not found"
            });
        } else if (result == -2) {
            res.json({
                result: result,
                message: "Wrong password"
            });
        } else if (result == -3) {
            res.json({
                result: result,
                message: "Config file not found"
            });
        } else {
            res.json({
                result: 0,
                name: result.name,
                path: result.path,
                content: result.content,
                created: result.created_formatted,
                modified: result.modified_formatted
            });
        }
    });

}

function getUserConfigs(username, password, name, callback) {

    User.find({
        username: username
    })
    .populate('configs')
    .exec(getUserConfigsAuth(password, name, callback));

}

function getUserConfigsAuth(password, name, callback) {

    return function(err, result) {
        if (err) { console.log(err); }

        if (result.length == 0) {
            callback(null, -1); // user not found
        } else {
            var user = result[0];
            bcrypt.compare(password, user.password, checkIfConfigExists(user, name, callback));
        }
    }

}

function checkIfConfigExists(user, name, callback) {

    return function(err, result) {
        if (err) { console.log(err); }

        if (!result) { // wrong password
            callback(null, -2);
        } else {
            var config = null;
            for (c of user.configs) {
                if (c.name == name) {
                    config = c;
                }
            }
            callback(null, { user: user, config: config });
        }
    }

}

exports.addConf = function(req, res, next) {

    getUserConfigs(req.body.username, req.body.password, req.body.name, function(err, result) {
        if (err) { return next(err); }

        if (result == -1) { // user not found
            res.json({
                result: -1,
                message: "User not found"
            });
        } else if (result == -2) {
            res.json({
                result: -2,
                message: "Invalid password"
            });
        } else {

            if (result.config == null) { // config does not exist
                var config = new Config({
                    name: req.body.name,
                    path: req.body.path,
                    content: req.body.content,
                    created: new Date(),
                    modified: new Date()
                });

                config.save(function(err) {
                    if (err) { return next(err); }

                    result.user.configs.push(config._id);
                    User.findByIdAndUpdate(result.user._id, result.user, {}, function(err, result) {
                        if (err) { return next(err); }

                        res.json({
                            result: 0
                        });
                    });
                });
            } else { // config already exists
                console.log('config already exists');
                result.config.content = req.body.content;
                result.config.modified = new Date();
                console.log(result.config);

                Config.findOneAndUpdate({_id: result.config._id}, {content: req.body.content, modified: new Date()}, {}, function(err, result) {
                    if (err) { return next(err); }

                    console.log(result);

                    res.json({
                        result: 0
                    });
                });
            }

        }
    });

}

function getUserAndDeleteConfig(username, password, name, callback) {

    User.find({ username: username })
    .populate('configs')
    .exec(authenticateAndDeleteConfig(password, name, callback));

}

function authenticateAndDeleteConfig(password, name, callback) {

    return function(err, result) {
        if (err) { console.log(err); }

        if (result.length == 0) { // user not found
            callback(null, -1);
        } else {
            var user = result[0];
            bcrypt.compare(password, user.password, verifyAuthenticationAndDeleteConfig(user, name, callback));
        }
    }

}

function verifyAuthenticationAndDeleteConfig(user, name, callback) {

    return function (err, result) {
        if (err) { console.log(err); }

        if (!result) { // wrong password
            callback(null, -2);
        } else {
            var config = null;
            for (c of user.configs) {
                if (c.name == name) {
                    config = c;
                }
            }
            console.log(config);
            if (config == null) {
                callback(null, -3); // config not found
            } else {
                callback(null, { user: user, config: config});
            }
        }
    }

}

exports.deleteConf = function(req, res, next) {

    getUserAndDeleteConfig(req.body.username, req.body.password, req.body.name, function(err, result) {

        if (err) { return next(err); }

        if (result == -1) { // user not found
            res.json({
                result: result,
                message: "User not found"
            });
        } else if (result == -2) { // wrong password
            res.json({
                result: result,
                message: "Wrong password"
            });
        } else if (result == -3) {
            res.json({
                result: result,
                message: "Config not found"
            });
        } else {

            var confId = result.config._id;

            Config.findOneAndRemove({ _id: result.config._id }, function(err) {
                if (err) { return next(err); }

                    var index = result.user.configs.indexOf(confId);
                    console.log(index);
                    result.user.configs.splice(index, 1);

                    User.findOneAndUpdate({ _id: result.user._id }, { configs: result.user.configs }, function(err) {
                        if(err) { return next(err); }

                        res.json({
                            result: 0
                        })
                    })

            });

        }
    });

}

function findUserAndCheckIfValid(username, password, email, callback) {

    if (email == null) {
        User.find({
            username: username
        }).exec(hashPasswordAndCheckIfValidUser(password, callback));
    } else {
        User.find({
            username: username,
            email: email
        }).exec(hashPasswordAndCheckIfValidUser(password, callback));
    }

}

function hashPasswordAndCheckIfValidUser(password, callback) {

    return function(err, result) {
        if (err) { console.log(err); }
        
        if (result.length == 0) {
            callback(null, -1);
        } else {
            bcrypt.compare(password, result[0].password, callback);
        }
    }

}

exports.validUser = function(req, res, next) {

    var j = JSON.parse(req.params.json);

    var email;
    if (j.email === undefined)
        email = null;
    else
        email = j.email;

    findUserAndCheckIfValid(j.username, j.password, email, function(err, result) {
        if (err) { return next(err); }

        if (result === true) {
            res.json({
                result: 0
            });
        } else {
            res.json({
                result: -1,
                message: "User not found"
            });
        }
    });

}