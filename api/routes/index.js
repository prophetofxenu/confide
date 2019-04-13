var express = require('express');
var router = express.Router();

var userController = require('../controllers/userController');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/config', userController.getConf);

router.post('/newuser', userController.newUser);

router.get('/validuser/:json', userController.validUser);

module.exports = router;
