<?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}
/**
 * @package   OctoMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
/**
 * Set the environment
 */
define('ENVIRONMENT', 0); # 0 - development; 1 - production

/**
 * Display the error wizard?
 */
define('ERROR_WIZARD', 1); # 1 - yes; 0 - no

/**
 * Global Encryption string for the blowfish algorithm (56 characters in length)
 */
define('BF_KEY', 'SADFo92jzVnzSj39IUYGvi7eL8v6RvJH9Cytuiouh347vCytdyUFl75R');

/**
 * Global Encryption salt for the blowfish algorithm (8 characters in length)
 */
define('BF_SALT', '92jzVnzS');

/**
 * API Octoms Encryption string (56 characters in length)
 */
define('API_KEY', 'SADFo92jzVnzSj39abcGvi6eL8v6RvJH8Cytuiouh547vCytdyUFl76R');

/**
 * API Octoms Project id
 */
define('API_PID', '1');

/**
 * API Unique ID
 */
define('API_UID', '4140c6f3-0716-400b-ae86-086d11777876');

/**
 * API - Individual access token
 */
define('API_TKN', '1345trew');

/**
 * Site information
 */
define('DOMAIN', false !== strpos($_SERVER['HTTP_HOST'], ':')
    ? substr($_SERVER['HTTP_HOST'], 0, strpos($_SERVER['HTTP_HOST'], ':'))
    : $_SERVER['HTTP_HOST']
); # the domain name
define('DOMAIN_PROTOCOL', 'http://'); # 'http://' or 'https://'

/**
 * Define the cookie parameters
 */
define('COOKIE_NAME', 'fervoare'); # the cookie name; default: 'octoms'
define('COOKIE_EXPIRE', 2592000);  # cookie expiration time; default: 30 days from now
define('COOKIE_PATH', '/');        # cookie path; default: "/"

/**
 * Define the default stage, controller and method
 */
define('DEF_STAGE', 'dispatch'); # stage name
define('DEF_CONTR', 'dispatch'); # controller name
define('DEF_METHO', 'index');    # methode name

/**
 * Define the database connection credentials
 */
define('DB_SERV', 'localhost'); # server
define('DB_USER', 'root');      # username
define('DB_PASS', '');          # password
define('DB_DATA', 'fervoare');  # database
define('DB_PREF', '');          # table prefix

/**
 * Fervoare CSM constants
 */
define('FERV_VERSION', '1.0.0'); # Fervoare CMS version
define('FERV_ADDONS', STAGES .DS . 'addons');	#addons folder
define('FERV_WIDGETS', STAGES .DS . 'widgets'); #widgets folder
define('FERV_TEMPLATE', 'default');	#the template
	
/* End Of File <constants.inc> */