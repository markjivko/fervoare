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
 * Admin controller
 */
class admin {

    /**
     * Themes: Select a theme for your website
     * @ro Teme: Selectează o temă pentru site-ul tău
     * 
     * @allow user
     * @param string $theme
     */
    public function themes($theme = null) {
        // List all themes
        if (is_null($theme)) {
            // Set the title
            $this->template->title(t(__FILE__, 1, 'Site theme'));

            // Use the uploadify module
            $this->template->uploadify(
                'themes',
                'themes',
                array(
                    '_replace_' => array($this->model, 'getThemes'),
                    //TODO Get from settings
                    'sizeLimit' => '20000000',
                    'fileExt' => array('zip')
                ),
                array($this->model, 'uploadify')
            );

            // Process actions
            if ($this->input->post()) {
                $this->model->setThemes();
            }

            // Load the view
            print $this->view->load('admin/partials/themes');
        }
        // Or edit the settings for the provided theme
        else {
            // Set the title
            $this->template->title(t(__FILE__, 2, '%1 theme settings', ucfirst($theme)));

            // Load the data
            if (is_null($data = $this->model->themeSettings($theme))) {
                $this->template->redirect('admin/design/themes');
            }

            // Load the data
            print $this->view->load('admin/partials/settings', array('data' => $data, 'theme' => $theme));
        }
    }// end function themes()
}

/* End Of File <admin.inc> */