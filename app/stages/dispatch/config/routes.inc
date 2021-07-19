<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
    // Blog
    $route['(:c=dispatch/open)/blog'] = '$1/content/serve/blog_index';
    $route['(:c=dispatch/open)/blog\.(:any)'] = '$1/content/serve/blog_index';
    $route['(:c=dispatch/open)/blog/(:any)'] = '$1/content/serve/blog_page/$2';
    // Search
    $route['(:c=dispatch/open)/search/(:any)'] = '$1/content/search/$2';
    // Front page
    $route['(:c=dispatch/open)/index'] = '$1/content/serve/index';
    $route['(:c=dispatch/open)/index\.(:any)'] = '$1/content/serve/index';
    // Post
    $route['(:c=dispatch/open)(:any)'] = '$1/content/serve/page$2';
    
    // Admin
    $route['(:c=dispatch/admin)/index'] = '$1/content/index';
    $route['(:c=dispatch/admin)/(:any)'] = '$1/$2';
    
/* End Of File <routes.inc> */