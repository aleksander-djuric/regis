##################################################
################ License Tracker #################
##################################################

DROP TABLE IF EXISTS `purchases`;
CREATE TABLE `purchases` (
  `cid` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8 NOT NULL,
  `org` varchar(255) CHARACTER SET utf8 NOT NULL,
  `org_unit` varchar(255) CHARACTER SET utf8 NOT NULL,
  `country` varchar(255) CHARACTER SET utf8 NOT NULL,
  `state` varchar(255) CHARACTER SET utf8 NOT NULL,
  `email` varchar(255) CHARACTER SET utf8 NOT NULL,
  `phone` varchar(255) CHARACTER SET utf8 NOT NULL,
  `reg_time` int(11) NOT NULL,
  `exp_time` int(11) NOT NULL,
  `num` int(11) NOT NULL,
  `comment` tinyblob NOT NULL,
  PRIMARY KEY (`cid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `licenses`;
CREATE TABLE `licenses` (
  `uid` varchar(16) NOT NULL,
  `ip` varchar(16) CHARACTER SET utf8 NOT NULL,
  `last_time` int(11) NOT NULL,
  `cert` blob NOT NULL,
  `serial` varchar(255) CHARACTER SET utf8 NOT NULL,
  `cid` int UNSIGNED NOT NULL,
  FOREIGN KEY (`cid`) REFERENCES customers(`cid`),
  PRIMARY KEY (`serial`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
