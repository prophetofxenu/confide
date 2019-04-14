var express = require('express');
var router = express.Router();

var userController = require('../controllers/userController');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/listconfs/:json', userController.listConfs);

router.get('/config/:json', userController.getConf);

router.post('/addconfig', userController.addConf);

router.post('/deleteconfig', userController.deleteConf);

router.post('/newuser', userController.newUser);

router.get('/validuser/:json', userController.validUser);

module.exports = router;
