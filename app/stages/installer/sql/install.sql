SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

--
-- Table structure for table `links`
--
CREATE TABLE IF NOT EXISTS `links` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `href` varchar(250) NOT NULL,
  `title` varchar(50) NOT NULL,
  `lang` varchar(3) NOT NULL DEFAULT 'en',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;

INSERT INTO `links` (`id`, `href`, `title`, `lang`) VALUES
(1, '/admin.html', 'Admin', 'en'),
(2, '/blog.html', 'Blog', 'en');

--
-- Table structure for table `menus`
--
CREATE TABLE IF NOT EXISTS `menus` (
  `name` varchar(50) NOT NULL,
  `content` text,
  `lang` varchar(3) NOT NULL DEFAULT 'en',
  UNIQUE KEY `name` (`name`,`lang`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `menus` (`name`, `content`, `lang`) VALUES
('default', 'ZUY1ZGpzRU9neUFRUlA5bFA2QVZxcGZocEg5Q282bWJnaVRBemZEdkxuaWh2YzI4emN5T2hjYVpNSUU0Yno2UnNSVXdCaEd2V3lTb0FmUjh1L0I1N05rN01nd2xkQVF0d3FyVnBsVTRQcjZhVEtuM1BxNGtibGZQeDA5ZUFuT0Zmd1ZLQ2tweklhNWI3QmExbHpHRTNDMjRiU2tYTnR3N3VRPT0=', 'en');

--
-- Table structure for table `posts`
--
CREATE TABLE IF NOT EXISTS `posts` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` int(3) NOT NULL DEFAULT '0',
  `categ` int(11) NOT NULL,
  `lang` varchar(3) NOT NULL DEFAULT 'en',
  `eq_to` int(11) DEFAULT NULL,
  `title` varchar(200) NOT NULL,
  `slug` varchar(200) NOT NULL,
  `content` text NOT NULL,
  `author_id` int(11) NOT NULL,
  `created` int(11) NOT NULL,
  `modified` int(11) NOT NULL,
  `status` int(3) NOT NULL DEFAULT '0',
  `options` text NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `type` (`type`,`lang`,`slug`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

INSERT INTO `posts` (`id`, `type`, `categ`, `lang`, `eq_to`, `title`, `slug`, `content`, `author_id`, `created`, `modified`, `status`, `options`) VALUES
(1, 1, 0, 'en', 0, 'Software as a service', 'saas', 'eF6NVbFu2zAQnfsXNxRBArh2k6FLjABtUqQZDBR1gc4UdZKIUKRKUjbcr+87SrZlNUMXWyKPx/fevTutS7N7oHfrmIJ39cPWV2mvApOKpChy2BnN69W4S9dbpba0ileKranv4+qGTA48HrMId9G4mpQrqUTUjsOBWo9HMo72jdHNORyHhxNckneSqC+iDqZLBq+FipIdifCHqBSUtQdqfExcLuklyXr0LSfTcqTAFYeATMkL/KvfvU/33n0ouZUcx0uH9UXOu0dc5UPLAXnfPH+C2tk+HgWJwx4VB9oYHbwELSlro7oOjJTgB3QR0kZPr87vnST9F07G8YuLD2DL5UqeBoKniOV6lYu0Hkp15YrY3V+u5atFJNxqSh5JFECtQiJfkba+L7Hddn1CdcDeoqC0N6lBWaqgUOFep35Webp+QeabBdirJELNdr8PuyXH1+S72ebTsAm2qp7QmUVtBLuE+cJYRs31K0ORWdCX8SKVlBiBw/yuxyEgm8UJ0lwCSqwb562vDyOOFqfnFH9u5PD/yowKD5VNhw6VFkcqDU+Ih2GIHnkjfnMPUGpgem0Nbl0QL+sl7YzC+p4LKoLfI3g0TgNQBaNCEEgK1QL9rH9AS1gcqJD0uPHCbQsQ17Yv5WJfVULtKDnURbSqZeu81qlD8NZSF7zAv9x8+rLZTs+fxTsvPn5+mryVvGPru1lMrVpTjRAXIB9Sr6z5M75DON+7wZEaWFThw7il+5ggRkBb2oFgY7opjuvHH5uTvfLKtPDxgB5qI6zzIr7InumCiYx80fdBS0cr54T19dcf329Evp03OmOpDNvy5JDzFQtqerydc0zxfMt4krLZYvp3b6IZuFhWId80lXEAKLyBDQsX1CRTzb4OqsPEfJvZc2aWZ8kIVPpwkuc/HH2a+2LB05wfvMhOHOVDJzVh6SvMlNTAVkkW0FIYLA7UpAPgI7DM5ptofep5mTzKGY5LesZIcqgrGLHG3InIqRJt3x4PeTpA71YZJ5H4xKhQcxS9witDR66zZlCm6wt0wzjrjqP6+P04XieDMbDCV+j97d3d8hNh7FjRFY169/Hudnkh018S9Ykn', 1, 1626630727, 1626631163, 1, '');

--
-- Table structure for table `settings`
--
CREATE TABLE IF NOT EXISTS `settings` (
  `key` varchar(50) NOT NULL,
  `value` text,
  `lang` varchar(3) NOT NULL DEFAULT 'en',
  UNIQUE KEY `key` (`key`,`lang`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

INSERT INTO `settings` (`key`, `value`, `lang`) VALUES
('general', 'ZUY1dFVrMXYyekFNL1N1RTduT2F0RnMzOWJpUHc0QU93MXBneDRDMjZVaU5KUmtpblN3WTh0OUgyYzdTQVFOOEVCOGZIL2xJbzEzZjJ0OXMzMXNqRklZZWhjd0QyM3RyV3VwdzdLVkVtN2ZYYkxYZ0ZaT0lqenMyRDJqWFJVS0ZUQm9vYm4xczZkZjI1Wkx4OWtZMTFodHJ1akUyNGxQa3FpVFBaMVhlL0VjWjZ6UktLYjRyc25mV1JBelRWSVg5aGZJaFlTYjROTFBoMmRHY1hhL0wwTnhrUDVRbUUzVC96cHBuNXhuMEUwZXd0Q2p2UURBeXRWQ2Zwc3hmM1krUFR4Vjg3d2xaNlFsaUVnZ1lSK3o3RTJRSzZVQVR2MHQ5U3hsOGhLUHpqVk5zYnFHeSttQkpXYlhaeDBaalVacFE3aWdUdzlGTFlVOHFTbFp0M09rakN2Q0pkUmRWR1Z6SHhsRmN5cE9MRzJzZU1lL2hxei9zMDNLZUk5WHM1MlBkS3R1SkRHeFhxNTJxajNYVnBMQjZFaHFjajJuVkxkWjA1V3cvV0ZQcXRwZWRscnU4OWw0d1BYZElJbk1yRGN5M3BQYmlEbmFaVU9DSURIUlE4OWc0cjQ5MjhxUTNBM1hoUnZiSTZnTGV3QS9zQndjL3NkYzlmZzZVZWI2S1RpOWFvUnNwa1E0VTFOekx4WnYraVIwMlZLZTBMNkY2WDFqSGxOdEJOOGl2WUIzdTRBTk5oUXVrNmllZFphd25ld3VvRzVJeDFQM1U4VnBMQVgzL0x6UzRGSytWNXo5OFV3bjg=', 'en'),
('seo', 'ZUY2TmtUMXZBakVNaHYrS2xibVZPRUNVaHExSTNaaGFkVVh1NFR2QzVRUEZBWW9RLzczT0VkckMxQ24yYXp1djh3VDFWSjlZVjA5YXRlUXBvbDNTVnlMUEpuZzFZejNXYXAyY3pXRTEwSXBOb21VeXlWS3ZETFY2cGJnUEdBbm1pN2RlRzVXdWpvNkhFRmVzWmlpM25Jd2VTSFdxVlZNRzFNem9TcVNKVnFGT3dYRVdoc1hIWWV4Z1kvWmR5T3Jvc2dnalN0TlpPaWJGWTBWY1I3Tk5aZGtxbS85ZENBd0RBaHUzdGZRQTFyVHI5SGlnZkVBZHZEd3pnVU9QTGJrYzhwRVRPVUMvZ3NYSEhKcUlqdVFKSFRRaHdqSHNJbmhoQTBZR295ZnBUeGpUYm50bDgybEQrOHRHOW4wUklZZlBwY1oyMStmalMzNmwxZGYrUyt0bjlzTHFCbDdHZEErdjBPbzk3bWlOeFB0OVRSQ2F4dFFHTGVRbXllRDJTOC9mL1AyMVlBPT0=', 'en');

--
-- Table structure for table `users`
--
CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(50) NOT NULL,
  `password` varchar(32) NOT NULL,
  `name` varchar(50) NOT NULL DEFAULT '',
  `email` varchar(50) NOT NULL DEFAULT '',
  `role` int(11) NOT NULL DEFAULT '2',
  `last_active` int(11) NOT NULL DEFAULT '0',
  `last_active_page` varchar(150) NOT NULL DEFAULT '',
  `log_out_flag` tinyint(1) NOT NULL DEFAULT '1',
  `last_ip` varchar(40) NOT NULL DEFAULT '',
  `created` int(11) NOT NULL DEFAULT '0',
  `details` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=MyISAM AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

INSERT INTO `users` (`id`, `username`, `password`, `name`, `email`, `role`, `last_active`, `last_active_page`, `log_out_flag`, `last_ip`, `created`, `details`) VALUES
(1, 'admin', '21232f297a57a5a743894a0e4a801fc3', 'Mark Jivko', 'stephino.team@gmail.com', 1, 1626632963, 'en::blog', 0, '::1', 0, NULL);