<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko https://markjivko.com
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
/*
 * Fervoare CMS Installer
 */
class install extends octoms {

    /**
     * Class constructor
     * 
     * @return install
     */
    public function __construct() {
        // Load the input core library
        $this->_('input', OMS_CL);

        // Load the model
        $this->_('install', OMS_M, 'model');

        // Load the template library
        $this->_('template', OMS_L);
    }// end function __construct()

    /**
     * Index
     */
    public function index() {
        // Use jQuery
        $this->template->js(
            array(
                WIZARD . WS . 'js' . WS . 'common' . WS . 'jquery.js',
                WIZARD . WS . 'js' . WS . 'common' . WS . 'jquery-ui.js',
                'custom' => true
            )
        );

        // Use the uniform and tooltip packages; load the jScript init procedures
        $this->template->jQuery(array('uniform', 'tooltip'), 'content', 'admin')->js(array(sprintf("$(document).ready(function(){\n%s\n});", $this->template->getOne('__jq__')), 'remote' => false));

        // Update
        if ($this->input->post()) {
            if ($this->model->update()) {
                $this->template->redirect('admin');
            }
        }

        // Load the template
        print $this->view->load('index');
    }// end function index()
}

/* End Of File <install.inc> */