<?php
/**
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

/**
 * Basic application information
 */
define('OCTOMS', '0.0.1');            # the framework version
define('ROOT', dirname(__FILE__));    # the framework root
define('APP_FOLDER', 'app');        # the application folder name (you may change this)

/**
 * Location constants
 */
define('DS', DIRECTORY_SEPARATOR);
define('WS', '/');
define('CORE', ROOT . DS . 'core');

/**
 * Load the rest of the constants
 */
require_once ROOT . '/core/config/constants.inc';

/**
 * Load the bootstrap utility
 */
require_once CORE_LIB . '/octoms' . EXT;

/**
 * It's the new OctoMS!
 */
new octoms();
    
/* End Of File <index.php> */