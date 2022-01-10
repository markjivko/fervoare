<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko https://markjivko.com
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @since         OctoMS 0.0.1
 */
    
/*
 * Admin controller
 * 
 * @package OctoMS
 * @subpackage Fervoare CMS
 */
class admin extends octoms {

    /**
     * Posts: Edit articles on the site
     * @ro Postări: Editează articolele de pe site
     * 
     * @allow user
     */
    public function posts($action = null) {
        // List is the default action
        if ($action == 'list')
            $this->template->redirect('admin/content/posts');
        if (is_null($action))
            $action = 'list';
        if (!in_array($action, array('list', 'add', 'edit'))) {
            $this->template->redirect('admin/content/posts');
        }

        switch ($action) {
            case 'list':
                $this->template->title(
                    t(__FILE__, 2, 'Site posts list')
                )->jQuery('tables');

                if ($this->input->post('delete')) {
                    $this->model->posts_delete($this->input->post('delete'));
                }

                print $this->view->load('admin/partials/posts_list', array('d' => $this->model->posts_list()));
                break;
                
            case 'edit':
                // We really need the id in args[0]
                $args = func_get_args();
                array_shift($args);
                if (count($args) == 0)
                    $this->template->redirect('admin/content/posts');

                // Set the title
                $this->template->title(
                    t(__FILE__, 3, 'Edit article #%1', $args[0])
                );

                // Load the ckeditor for the forms
                $this->template->jQuery('ckeditor')
                        ->css(array('posts', 'tpl' => false))
                        ->js(array('posts', 'tpl' => false));

                // Uploading some files?
                $this->template->uploadify('posts', 'posts' . DS . $args[0]);

                // Process the request
                if ($this->input->post() && !$this->input->file()) {
                    $this->model->posts_update($args);
                }

                // Or just display the template
                $d = $this->model->posts_read($args);
                print $this->view->load(
                    'admin/partials/posts_edit',
                    array(
                        'eq_to' => $this->model->posts_eq_to($d['data']['eq_to']),
                        'd' => $d
                    )
                );
                break;
                
            case 'add':
                $id = $this->model->posts_add();
                $this->template->redirect('admin/content/posts' . (is_numeric($id) ? '/edit/' . $id : ''));
                break;
        }
    }// end function posts()

    /**
     * Regions: Edit the theme's regions
     * @ro Regiuni: Editează regiunile temei
     * 
     * @allow user
     */
    public function regions() {
        // Set the title
        $this->template->title(
                t(__FILE__, 4, 'Edit the theme\'s regions')
        );

        // Load the ckeditor
        $this->template->jQuery('ckeditor', 'content');

        // Fetch all available template files
        $r = array();
        if (count($tpl = $this->model->region_list()) > 0) {
            foreach ($tpl as $t) {
                $r[$t] = $this->model->region_get($t);
            }
        }

        // Load the template
        print $this->view->load('admin/partials/regions', array('tpl' => $r));

        // Update the template
        if ($this->input->post()) {
            $this->model->region_update();
        }
    }// end function regions()

    /**
     * Links: Edit links used in your menu
     * @ro Legături: Editează legăturile de pe site
     * 
     * @allow user
     */
    public function links($action = null) {
        // List is the default action
        if ($action == 'list')
            $this->template->redirect('admin/content/links');
        if (is_null($action))
            $action = 'list';
        if (!in_array($action, array('list', 'add', 'edit', 'delete'))) {
            $this->template->redirect('admin/content/links');
        }

        switch ($action) {
            case 'list':
                $this->template->title(
                        t(__FILE__, 5, 'Links list')
                )->jQuery('tables');

                if ($this->input->post('delete')) {
                    $this->model->links_delete($this->input->post('delete'));
                }

                print $this->view->load('admin/partials/links_list', array('d' => $this->model->links_list()));
                break;
                
            case 'edit':
                // We really need the id in args[0]
                $args = func_get_args();
                array_shift($args);
                if (count($args) == 0)
                    $this->template->redirect('admin/content/posts');

                // Load the ckeditor for the forms
                $this->template->jQuery('ckeditor');
                $this->template->title(
                        t(__FILE__, 6, 'Edit link #%1', $args[0])
                );

                // Process the request
                if ($this->input->post()) {
                    $this->model->links_update($args);
                }

                // Or just display the template
                print $this->view->load('admin/partials/links_edit', array('d' => $this->model->links_read(isset($args[0]) ? $args[0] : null)));
                break;
                
            case 'add':
                $id = $this->model->links_add();
                $this->template->redirect('admin/content/links' . (is_numeric($id) ? '/edit/' . $id : ''));
                break;
        }
    }// end function links()

    /**
     * Menus: Edit the site's menus
     * @ro Meniuri: Editează meniurile site-ului
     * 
     * @allow user
     */
    public function menus() {
        // Set the title
        $this->template->title(
                t(__FILE__, 1, 'Menu management')
        )->jQuery('menu');

        // Any update?
        if ($this->input->post()) {
            $this->model->menu_update();
        }

        // Read the menu
        $menu = $this->model->menu_read();

        // Load the template
        print $this->view->load('admin/partials/menu', $menu);
    } // end function menus()
}
    
/* End Of File <admin.inc> */