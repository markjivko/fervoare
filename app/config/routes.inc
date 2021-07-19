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
    
$route['blog'] = 'dispatch/dispatch/open/blog';
$route['blog/(:any)'] = 'dispatch/dispatch/open/blog/$1';

$route['blog'] = 'dispatch/dispatch/open/blog';
	$route['blog/(:any)'] = 'dispatch/dispatch/open/blog/$1';

$route['blog'] = 'dispatch/dispatch/open/blog';
	$route['blog/(:any)'] = 'dispatch/dispatch/open/blog/$1';

$route['admin'] = 'dispatch/dispatch/admin/index';
$route['admin/(:any)'] = 'dispatch/dispatch/admin/$1';

$route['search/(:any)'] = 'dispatch/dispatch/open/search/$1';
$route['testarea'] = 'dispatch/test/index';

$route['(:blank)'] = 'dispatch/dispatch/open/index';
$route['(:any)'] = 'dispatch/dispatch/open/$1';
    
/* End Of File <routes.inc> */