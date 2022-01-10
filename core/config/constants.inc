<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/**
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

/**
 * Basic ini settings
 */
ini_set('register_globals', 0);
ini_set('session.auto_start', 0);

/**
 * Allowed Framework languages
 * 
 * Comma-separated; no spaces; 
 * DEFAULT_LANGUAGE must be present;
 */
define('LANGUAGES', 'en,ro');
define('DEFAULT_LANGUAGE', 'en');

/**
 * Employee types
 */
define('OMS_L', 1);     # library
define('OMS_H', 2);     # helper
define('OMS_M', 3);     # model
define('OMS_CL', 4);    # core library
define('OMS_CH', 5);    # core helper

/**
 * File extension
 */
define('EXT', '.inc');

if(!file_exists(ROOT . DS . APP_FOLDER)) {
    die('Invalid application folder "' . APP_FOLDER . '".');
}

/**
 * Core configuration path
 */
define('CORE_CONFIG', CORE . DS . 'config');

/**
 * Core helpers path
 */
define('CORE_HELP', CORE . DS . 'helpers');

/**
 * Core libraries path
 */
define('CORE_LIB', CORE . DS . 'libraries');

/**
 * Core error templates path
 */
define('CORE_ERR', CORE . DS . 'errors');

/**
 * WebRoot
 */
define('WEBROOT', str_replace('/index.php', '', $_SERVER['SCRIPT_NAME']));

/**
 * Application server path
 */
define('APP_ROOT', ROOT . DS . APP_FOLDER);

/**
 * Application config path
 */
define('APP_CONFIG', APP_ROOT . DS . 'config');

/**
 * Application helpers path
 */
define('APP_HELP', APP_ROOT . DS . 'helpers');

/**
 * Application libraries path
 */
define('APP_LIB', APP_ROOT . DS . 'libraries');

/**
 * Application templates path
 */
define('APP_TPL', APP_ROOT . DS . 'templates');

/**
 * Application error templates path
 */
define('APP_ERR', APP_ROOT . DS . 'errors');

/**
 * Stages server path
 */
define('STAGES', APP_ROOT . DS . 'stages');

/**
 * Application web path
 */
define('APP_WEBROOT', WEBROOT . WS . APP_FOLDER);

/**
 * Wizard webroot
 */
define('WIZARD', WEBROOT . WS . 'core' . WS . 'wizard' . WS . 'views');

/**
 * The wizard launches automatically on any page on your website you query with
 * ?:{DEBUG_KEYWORD}
 * if the application is in development mode (ENVIRONMENT = 0)
 * 
 * You may change this to anything you want for special cases where you use ?debug:{something}
 * as a _GET key in your application.
 * Use only letters [a-zA-Z] and numbers [0-9].
 */
define('DEBUG_KEYWORD', 'debug');

/**
 * Get the application-specific constants
 */
require_once APP_CONFIG . DS . 'constants' . EXT;
    
/* End Of File <constants.inc> */