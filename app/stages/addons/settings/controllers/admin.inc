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
 * Settings controller for the admin section
 */
class admin extends octoms {

    /**
     * General: General site settings
     * 
     * @allow user
     */
    public function index() {
        // Set the title
        $this->template->title('General site settings');

        // Save the data
        if ($this->input->post()) {
            $this->model->general_update();
        }

        // Display the data
        print $this->view->load('admin/partials/general', array('d' => $this->model->general_read()));
    }// end function index()

    /**
     * SEO: Search Engine Optimization settings
     * 
     * @allow user
     */
    public function seo() {
        // Set the title
        $this->template->title('Search Engine Optimization settings');

        // Load the js
        $this->template->js(array('seo', 'tpl' => false));

        // Save the data
        if ($this->input->post()) {
            $this->model->seo_update();
        }

        // Display the data
        print $this->view->load('admin/partials/seo', array('d' => $this->model->seo_read()));
    }// end function seo()

    /**
     * Profile: Manage your profile
     * 
     * @allow user
     */
    public function profile() {
        // Set the title
        $this->template->title('Manage my profile');

        // Display the data
        print $this->view->load('admin/partials/users_edit', array('data' => $this->auth->user()));

        // Save the data
        if ($this->input->post()) {
            $this->model->users_update();
        }
    }// end function profile()

    /**
     * Users: Manage users of this site
     */
    public function users() {
        // Set the title
        $this->template->title('Manage users of this site');

        // Get the arguments
        $args = func_get_args();

        // Return to index otherwise
        if (isset($args[0]) && !in_array($args[0], array('add', 'edit'))) {
            $this->template->redirect('admin/settings/users');
        }

        // Save the data
        if ($this->input->post()) {
            $this->model->users_update($args);
        }

        // Load the appropiate template
        if (isset($args[0])) {
            switch ($args[0]) {
                case 'add':
                    // Get the new id
                    $id = $this->model->users_add();

                    // Redirect
                    $this->template->redirect('admin/settings/users/edit/' . $id);
                    break;
                
                case 'edit':
                    // If no valid id is provided, go to the index
                    if (!isset($args[1]) || is_null($data = $this->model->users_read($args[1]))) {
                        $this->template->redirect('admin/settings/users');
                    }

                    // Load the JS
                    $this->template->js(array('users', 'tpl' => false));

                    // Load the template
                    print $this->view->load('admin/partials/users_edit', array('data' => $data));
                    break;
            }
        }
        // Or show the index
        else {
            // Get the data
            $d = $this->model->users_read();

            // Display the data
            print $this->view->load('admin/partials/users', array('d' => $d));

            // If there are tables, make them nice
            if (isset($d['others']) && count($d['others']) > 0) {
                // Load the tables
                $this->template->jQuery('tables', 'content');

                // Load the 'users' CSS and JS
                $this->template
                    ->css(array('users', 'tpl' => false))
                    ->js(array('users', 'tpl' => false));
            }
        }
    }// end function users()
}
    
/* End Of File <admin.inc> */