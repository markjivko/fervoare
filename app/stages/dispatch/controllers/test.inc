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
    
/*
 * Test class
 * 
 * 
 */
class test extends octoms
{
    /**
     * Index page
     */
    public function index()
    {
        header('content-type:text/plain');
        print 'Hello ' . mt_rand(10, 1000) . ' times :)';
    }// end function __construct()

}
    
/* End Of File <test.inc> */