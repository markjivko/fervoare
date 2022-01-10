<?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @copyright Copyright 2012, Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

class main extends octoms {

    /**
     * <h3>About OctoMS</h3><br/>
     * This is a light-weight PHP framework inspired by <a rel="noreferrer" href="https://codeigniter.com/" target="_blank">CodeIgniter</a>.<br/>
     * If the app is in <b>development mode</b> this wizard interface will be shown for <a href="/error">uncaught exceptions</a> and <a href="/?:debug">on-demand</a>.<br/>
     * This interface contains a debugging window (with routing rules, arguments, source code, request headers and other information) and a search window.<br/>
     * You can search through the entire application or specify a search location: <ul>
     * <li>{term} <b>-core</b> To look inside the core libraries and helpers</li>
     * <li>{term} <b>-app</b> To look inside the app libraries and helpers</li>
     * <li>{term} <b>-stage={stage}</b> To look inside a specific app stage's controllers and models</li>
     * </ul>
     * 
     * @param ...$arg Path components
     * @return null Loads the index.tpl file and displays it
     */
    public function index($arg = null) {
        /* @var $view view_cl*/
        $view = $this->_('view');
        
        // Uncaught exception
        if ('error' === $arg) {
            throw new Exception('Something went wrong', 13);
        }
        
        // Display the template
        $view->display(
            'index.tpl', 
            array('args' => func_get_args())
        );
    }// end function
}
    
/* End Of File <test.inc> */