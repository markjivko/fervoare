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
 * Admin model
 */
class admin_m extends octoms {

    /**
     * Constructor
     */
    public function __construct() {
        // Load the ZIP core library
        $this->_('zip', OMS_CL);
    }// end function __construct()

    /**
     * Return the uploadify custom fields
     */
    public function uploadify($arg = null) {
        if ($arg === true) {
            return array(
                t(__FILE__, 1, 'Theme'),
                t(__FILE__, 2, 'Information'),
                t(__FILE__, 3, 'Actions')
            );
        } elseif (is_array($arg)) {
            // Return an installed theme
            if (isset($arg['slug'])) {
                $name = isset($arg['about']['name']) ? $arg['about']['name'] : $arg['slug'];
                $description = isset($arg['about']['description']) ? $arg['about']['description'] : '';
                if (!is_null($arg['about'])) {
                    // Remove these keys
                    unset($arg['about']['name']);
                    unset($arg['about']['description']);

                    // Add the author
                    if (isset($arg['about']['author'])) {
                        $description .= '<br/>' . t(__FILE__, 4, 'by') . ' ' . (isset($arg['about']['website']) ? a($arg['about']['author'], $arg['about']['website'], array('target' => '_blank')) : $arg['about']['author']);
                        unset($arg['about']['author']);
                        unset($arg['about']['website']);
                    }

                    // Social sites?
                    if (isset($arg['about']['facebook'])) {
                        $description .= '<br/>' . a('Facebook Page', 'http://facebook.com/' . $arg['about']['facebook'], array('target' => '_blank'));
                        unset($arg['about']['facebook']);
                    }
                    if (isset($arg['about']['twitter'])) {
                        $description .= '<br/>' . a('Twitter Page', 'http://twitter.com/' . $arg['about']['twitter'], array('target' => '_blank'));
                        unset($arg['about']['twitter']);
                    }
                    if (isset($arg['about']['googleplus'])) {
                        $description .= '<br/>' . a('Google+ Page', 'http://plus.google.com/' . $arg['about']['googleplus'], array('target' => '_blank'));
                        unset($arg['about']['googleplus']);
                    }

                    // Any other information?
                    if (count($arg['about']) > 0) {
                        foreach ($arg['about'] as $key => $value) {
                            $description .= '<br/>' . $key . ' &rArr; ' . $value;
                        }
                    }
                }
                // Return an installed theme row
                return array(
                    action_radio(
                            'activate',
                            $arg['slug'],
                            $arg['active'],
                            t(__FILE__, 5, 'Activate theme "%1"', $name),
                            array(
                                'confirm' => t(__FILE__, 29, 'Are you sure you want to activate theme "%1"?', $name)
                            )
                    ) .
                    $name . tag('div', array('class' => 'theme_img', 'style' => 'background-image:url(' . baseurl(APP_FOLDER . WS . 'templates' . WS . $arg['slug']) . '/thumbnail.png)')),
                    $description,
                    (!is_null($arg['settings']) ? link_btn(t(__FILE__, 6, 'Settings'), $arg['slug']) : '') .
                    ($arg['slug'] != 'default' ? action_btn(t(__FILE__, 7, 'Uninstall'), $arg['slug'], 'uninstall', array('confirm' => t(__FILE__, 30, 'Are you sure you want to uninstall theme "%1"?', $name))) : '') .
                    action_btn(t(__FILE__, 8, 'Reset'), $arg['slug'], 'reset', array('confirm' => t(__FILE__, 28, 'Are you sure you want to reset theme "%1" to its default state? Data from the theme\'s info.ini file will be re-read.', $name)))
                );
            }
            // Or an uploaded ZIP
            else {
                // Return a ZIP row
                return array(
                    a($arg['name'], $arg['url'], array('target' => '_blank')),
                    t(__FILE__, 9, 'This package is not installed. Size: <i>%1KB</i>', number_format($arg['size'] / 1024, 2)),
                    action_btn(t(__FILE__, 10, 'Install'), $arg['name'], 'install') .
                    action_btn(t(__FILE__, 11, 'Delete'), $arg['name'], 'uploadify_delete')
                );
            }
        }
    }// end function uploadify()

    /**
     * Get available themes
     */
    public function getThemes() {
        // Get ZIP files uploaded
        $zip = $this->input->file_list('themes');
        if (is_null($zip))
            $zip = array();

        // Prepare the installed themes array
        $installed = array();

        // Get installed themes
        clearstatcache();
        if (false !== $files = @scandir(APP_TPL)) {
            foreach ($files as $f) {
                if (is_dir(APP_TPL . DS . $f) && !in_array($f, array('.', '..'))) {
                    $installed[] = array(
                        'slug' => $f,
                        'about' => $this->settings->get("general.template.{$f}.about"),
                        'active' => $this->settings->get('general.template') == $f,
                        'settings' => $this->settings->get("general.template.{$f}.settings")
                    );
                }
            }
        }

        // Remove the zip files for installed themes
        if (count($zip) > 0) {
            foreach ($installed as $i) {
                foreach ($zip as $key => $z) {
                    if (substr($z['name'], 0, strrpos($z['name'], '.')) == $i['slug']) {
                        unset($zip[$key]);
                    }
                }
            }
        }

        // Return the result
        return array_merge($zip, $installed);
    }// end function getThemes()

    /**
     * Update a theme's information
     * 
     * @param string $theme
     */
    public function updateThemes($theme = null) {
        // Is the directory there?
        if (file_exists(APP_TPL . DS . $theme)) {
            // Find the info.ini
            if (!file_exists($f = APP_TPL . DS . $theme . DS . 'info.ini')) {
                $this->template->api(
                        t(__FILE__, 12, 'Invalid theme. Please provide a info.ini file in the root of your theme.'),
                        false
                );
                return;
            } else {
                // Get the information
                if (false === $info = @parse_ini_file($f, true)) {
                    $this->template->api(
                            t(__FILE__, 13, 'Your info.ini file is not well formatted.'),
                            false
                    );
                    return;
                }

                // And the name and description
                if (!isset($info['about']['name']) || !isset($info['about']['description'])) {
                    $this->template->api(
                            t(__FILE__, 14, 'Your info.ini about section must contain the "name" and "description" keys.'),
                            false
                    );
                    return;
                }

                // We need the "about" section
                if (!isset($info['about'])) {
                    $this->template->api(
                            t(__FILE__, 15, 'Your info.ini must contain the "about" key.'),
                            false
                    );
                    return;
                }

                // Clean the data, if any
                $this->settings->delChildren('general.template.' . $theme);

                // Set the settings
                if (isset($info['settings'])) {
                    $this->settings->set('general.template.' . $theme . '.settings', $info['settings']);
                }

                // Set the "about" information
                $this->settings->set('general.template.' . $theme . '.about', $info['about']);
            }
        } else {
            // Folder not found
            $this->template->api(
                    t(__FILE__, 16, 'Folder "%1" not found.', APP_TPL . DS . $theme),
                    false
            );
        }
    }// end function updateThemes()

    /**
     * Process the actions
     */
    public function setThemes() {
        // Install a theme
        if ($this->input->post('install')) {
            // Try an extraction
            try {
                // Extract the theme
                $this->zip->extract(
                        ROOT . DS . 'upload' . DS . 'themes' . DS . $this->input->post('install'),
                        APP_TPL
                );
            } catch (Exception $e) {
                // Keep the error message for debugging purposes
                $this->template->api_cnt(
                        $e->getMessage()
                );
            }

            // Set the theme name
            $theme = preg_replace('/^([^.]+)\..*/i', '$1', $this->input->post('install'));

            // Update the information
            $this->updateThemes($theme);

            // Set the install message
            $this->template->api(
                t(__FILE__, 17, 'Installed the "%1" theme successfully.', $theme)
            );
        }

        // Reset the theme information
        if ($this->input->post('reset')) {
            // Update the theme
            $this->updateThemes($this->input->post('reset'));

            // Set the update message
            $this->template->api(
                t(__FILE__, 18, 'Reset the "%1" theme successfully.', $this->input->post('reset'))
            );
        }

        // Uninstall a theme
        if ($this->input->post('uninstall')) {
            // Uninstall
            if (is_dir($dir = APP_TPL . DS . $this->input->post('uninstall'))) {
                // Prevent deletion of the default template
                if ($this->input->post('uninstall') == FERV_TEMPLATE) {
                    $this->template->api(
                        t(__FILE__, 19, 'You cannot remove the default template.'),
                        'info'
                    );
                    return;
                }

                // Remove the directory
                if (@remove_dir($dir)) {
                    // Remove information about it
                    $this->settings->delChildren('general.template.' . $this->input->post('uninstall'));

                    // Send a success message
                    $this->template->api(
                        t(__FILE__, 20, 'Uninstalled the "%1" theme successfully.', $this->input->post('uninstall'))
                    );
                } else {
                    // Directory could not be removed
                    $this->tempalte->api(
                        t(__FILE__, 21, 'Could not remove directory "%1".', $dir),
                        false
                    );
                    return;
                }

                // If this was active, reset to the default
                if ($this->input->post('uninstall') == $this->settings->get('general.template')) {
                    $this->settings->set('general.template', FERV_TEMPLATE);
                }
            } else {
                $this->template->api(
                    t(__FILE__, 22, 'Invalid theme to uninstall "%1".', $this->input->post('uninstall')),
                    false
                );
                return;
            }
        }

        // Activate a theme
        if ($this->input->post('activate')) {
            if ($this->settings->get('general.template') == $this->input->post('activate')) {
                $this->template->api(
                    t(__FILE__, 23, 'Theme "%1" already active.', $this->input->post('activate')),
                    'info'
                );
            } else {
                // Activate
                if (is_dir($dir = APP_TPL . DS . $this->input->post('activate'))) {
                    // Set the template
                    $this->settings->set('general.template', $this->input->post('activate'));

                    // Let'em know
                    $this->template->api(
                        t(__FILE__, 24, 'Theme "%1" activated successfully.', $this->input->post('activate'))
                    );
                } else {
                    $this->template->api(
                        t(__FILE__, 25, 'Theme "%1" not found.', $this->input->post('activate')),
                        false
                    );
                    return;
                }
            }
        }

        // Update the table
        if (count($this->input->post(array('install', 'uninstall', 'reset', 'activate'))) > 0 && is_array($themes = $this->getThemes())) {
            foreach ($themes as $k => $v) {
                $themes[$k] = $this->uploadify($v);
            }
            $this->template->api(array('updateTable'))->api_cnt($themes);
        }
    }// end function setThemes()

    /**
     * Theme Settings
     * 
     * @param string $theme
     * @return array - theme settings, null on error
     */
    public function themeSettings($theme = null) {
        // The theme is mandatory
        if (is_null($theme))
            return null;

        // Update?
        if ($this->input->post()) {
            $settings = $this->settings->get('general.template.' . $theme . '.settings');
            if (!is_null($settings) && is_array($settings)) {
                foreach ($this->input->sg('post') as $key => $value) {
                    if (in_array($key, array_keys($settings))) {
                        if (is_array($settings[$key])) {
                            $settings[$key] = explode(',', $value);
                        } else {
                            $settings[$key] = $value;
                        }
                    }
                }
                // Update the settings
                $this->settings->set('general.template.' . $theme . '.settings', $settings);

                // Show a message
                $this->template->api(
                    t(__FILE__, 26, 'Settings updated successfully for theme "%1".', $theme)
                );
            } else {
                // What are you trying to set?
                $this->template->api(
                    t(__FILE__, 27, 'There are no settings for theme "%1".', $theme),
                    false
                );
            }
        }

        // Get the settings from the database
        return $this->settings->get('general.template.' . $theme . '.settings');
    }// end function themeSettings()
}

/* End Of File <admin.inc> */