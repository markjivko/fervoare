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
 * Dispatch
 * 
 * @package OctoMS
 * @subpackage Fervoare CMS
 */
class dispatch extends octoms {
    /*
     * Allow deployment of method?
     * Set by verifying user permissions
     * 
     * @var boolean
     */
    public $allowMethod = true;

    /*
     * Addon methods description
     * 
     * @var array 
     */
    public $addonMethods = array();

    /*
     * Class constructor
     */
    public function __construct() {
        // Load the template library
        $this->_('template', OMS_L);

        // This CMS uses SQL to store user information
        try {
            // Load the SQL core library
            $this->_('sql', OMS_CL);

            // Load the settings library
            $this->_('settings', OMS_L);
        } catch (Exception $e) {
            // Could not connect to the database
            if ($e->getCode() == 500) {
                // Production mode; Return an error page
                if (ENVIRONMENT) {
                    octoms('error', OMS_CL)->page(
                        500,
                        'Please revert the OctoMS Framework to Development mode 
                        to initiate the Fervoare CMS install utility.'
                    );
                } else {
                    // Load the view core library
                    $this->_('view', OMS_CL);

                    // Load the HTML core helper
                    $this->_('html', OMS_CH);

                    // Get the Installer up and running
                    print $this->view->addon('installer', 'install', 'index');
                    exit();
                }
            }
            // The settings library triggered an error
            else {
                // Production mode; Return an error page
                if (ENVIRONMENT) {
                    octoms('error', OMS_CL)->page(500, $e->getMessage());
                } else {
                    // Load the view core library
                    $this->_('view', OMS_CL);

                    // Load the HTML core helper
                    $this->_('html', OMS_CH);

                    // Get the Installer up and running
                    print $this->view->addon('installer', 'install', 'index');
                    exit();
                }
            }
        }
    }// end function __construct()

    /**
     * Pre ops - Called before the method
     */
    protected function _pre() {
        // Perform a search?
        // TODO security checks; no floods; session flash key
        if ($this->input->post('search')) {
            $this->template->redirect(
                implode(
                    WS,
                    array(
                        'search',
                        urlencode($this->input->post('search')),
                        1
                    )
                )
            );
        }

        // Use jQuery
        $this->template->js(
            array(
                WIZARD . WS . 'js' . WS . 'common' . WS . 'jquery.js',
                WIZARD . WS . 'js' . WS . 'common' . WS . 'jquery-ui.js',
                'custom' => true
            )
        );

        // Load the session core library
        $this->_('session', OMS_CL);

        // Force authentification for scripts such as uploadify
        if (isset($_POST['COOKIE']) && isset($_POST['HTTP_USER_AGENT'])) {
            $_COOKIE[COOKIE_NAME] = str_replace(' ', '+', $_POST['COOKIE']);
            $_SERVER['HTTP_USER_AGENT'] = $_POST['HTTP_USER_AGENT'];
        }

        // Start the session
        $this->session->start();

        // Load the authorization library
        $this->_('auth', OMS_L);

        // Mark the user as active 
        $this->auth->mark_active();

        // Load this for the administrative section only
        if ($this->template->conf('area') != 'admin')
            return;

        // Use the uniform and tooltip packages
        // TODO decide which modules to load for each page from the settings
        $this->template->jQuery(array('tooltip', 'uniform'), 'content', 'admin');

        // Load the 'fervoare' jQuery toolkit; do not index this page
        $this->template->jQuery('fervoare', 'content')->meta('robots', 'noindex,nofollow');

        // Log out?
        if ($this->input->post('logout')) {
            // Add a log_out flag
            $this->auth->flag_log_out();

            // Log out
            $this->auth->log_out();

            // Show the login form
            $this->template->redirect(implode(WS, octoms::$oms_url_segments));
        }

        // If the user is not logged in, show a login form
        if (!$this->auth->is_logged()) {
            // Back to the defaults
            octoms::$oms_info['stage'] = 'dispatch';
            octoms::$oms_info['controller'] = 'dispatch';
            octoms::$oms_info['method'] = __FUNCTION__;
            octoms::$oms_info['parameters'] = func_get_args();

            $this->template
                    ->conf('page', 'login')
                    ->title(t(__FILE__, 6, 'Fervoare CMS: Log In'));

            // Attempting login?
            if ($this->input->post('username') && $this->input->post('password')) {
                if (
                    $this->auth->log_in(
                        $this->input->post('username', 'all'),
                        $this->input->post('password')
                    )
                ) {
                    $this->template->redirect(implode(WS, octoms::$oms_url_segments));
                } else {
                    $this->template->region('error', t(__FILE__, 7, 'Invalid credentials.'));
                }
            } elseif ($this->input->post('username') || $this->input->post('password')) {
                $this->template->region('error', t(__FILE__, 8, 'All fields are required.'));
            }

            // Disable AJAX queries here
            if (!is_null($this->input->post('#*#')))
                $this->input->del('post', '#*#');

            // Post processing
            $this->_post();

            // Load the template
            $this->template->flush();
        } else {
            // Get all the addons from the filesystem
            $r = array();

            // Store the addon methods
            $addon_methods = null;

            // Forced redirect?
            $forced_redirect = false;

            // Get all the addons
            if (false !== $h = opendir(STAGES . DS . 'addons')) {
                while (false !== ($file = readdir($h))) {
                    if (is_dir(STAGES . DS . 'addons' . DS . $file) && !in_array($file, array('.', '..'))) {
                        // Prepare the addon information
                        $data = array(
                            'name' => ucfirst(strtolower($file)),
                            'slug' => strtolower($file),
                            'description' => t(__FILE__, 1, 'The %1 addon', $file),
                            'author' => t(__FILE__, 2, 'unknown'),
                            'version' => t(__FILE__, 2, 'unknown')
                        );

                        // Use the user supplied information
                        if (file_exists($info = STAGES . DS . 'addons' . DS . $file . DS . 'info.ini') && false !== $info = parse_ini_file($info)) {
                            // Set the name
                            isset($info['name']) ? ($data['name'] = $info['name']) : null;

                            // Set the description
                            isset($info['description']) ? ($data['description'] = $info['description']) : null;

                            // Overwrite for the local language
                            if ('en' !== $lang = lang()) {
                                // Set the name
                                isset($info['name_' . $lang]) ? ($data['name'] = $info['name_' . $lang]) : null;

                                // Set the description
                                isset($info['description_' . $lang]) ? ($data['description'] = $info['description_' . $lang]) : null;
                            }

                            // Set the Author
                            isset($info['author']) ? ($data['author'] = $info['author']) : null;

                            // Set the Version
                            isset($info['version']) ? ($data['version'] = $info['version']) : null;
                        }

                        // Is this method active?
                        $data['selected'] = ($data['slug'] == $this->template->conf('addon'));

                        // Append the addon if it has accesible methods
                        if (count($a_m = $this->_methods($file)) > 0) {
                            // Append the data
                            $r[] = $data;

                            // Try a forced redirection?
                            if ($forced_redirect) {
                                $this->template->redirect('admin/' . $file);
                            }
                        }

                        // Store this information
                        if ($file == $this->template->conf('addon')) {
                            // Acces granted?
                            if (count($a_m) > 0) {
                                // Save the addon_methods
                                $addon_methods = $a_m;
                            } else {
                                // Redirect as soon as possible
                                $forced_redirect = true;
                            }
                        }
                    }
                }
            }

            // Set the "addons" region
            $this->template->region(
                'addons',
                $this->view->load(
                    'partials/addons',
                    array(
                        'r' => $r
                    ),
                    null,
                    'templates' . DS .
                    $this->template->conf('template') . DS .
                    $this->template->conf('area')
                )
            );

            // Set the "methods" region
            $this->template->region(
                'methods',
                $this->view->load(
                    'partials/methods',
                    array(
                        'r' => $addon_methods,
                        'addon' => $this->template->conf('addon')
                    ),
                    null,
                    'templates' . DS .
                    $this->template->conf('template') . DS .
                    $this->template->conf('area')
                )
            );
        }
    }// end function _pre()

    /**
     * Dispatch calls to specific addons
     * 
     * @param string $area - Fervoare CMS Stage area
     * @return array $args - Method arguments
     */
    public function __call($area = '', $args = array()) {
        // Redirect to the extension-enabled page?
        if (false === octoms::$oms_info['ext']) {
            $defaultExt = $this->settings->get('seo.general_extension');
            if ($defaultExt != null && $defaultExt != '') {
                // Redirect to the extension page
                if ('' !== $page = implode(WS, octoms::$oms_url_segments)) {
                    $this->template->redirect($page . '.' . $defaultExt);
                }
            }
        }

        // Prepare the addon
        $addon = array_shift($args);

        // Load the addon
        if (file_exists(FERV_ADDONS . DS . $addon . DS . 'controllers' . DS . $area . EXT)) {
            // Load the controller
            require_once FERV_ADDONS . DS . $addon . DS . 'controllers' . DS . $area . EXT;

            // Controller found!
            if (class_exists($area)) {
                // Load the model, if there is one
                if (file_exists(FERV_ADDONS . DS . $addon . DS . 'models' . DS . $area . EXT)) {
                    require_once FERV_ADDONS . DS . $addon . DS . 'models' . DS . $area . EXT;
                }

                // The first argument is the method
                $method = is_null($method = array_shift($args)) ? 'index' : $method;

                // Set debugging info for the OctoMS framework
                parent::$oms_info['stage'] = 'addons' . DS . $addon;
                parent::$oms_info['controller'] = $area;
                parent::$oms_info['method'] = $method;
                parent::$oms_info['parameters'] = $args;

                // Prevent access to secret methods
                if (substr($method, 0, 1) == '_') {
                    $this->error->page(404);
                }

                // Set the template defaults
                $general_template = $this->settings->get('general.template');
                $this->template
                    ->conf('addon', $addon)
                    ->conf('area', $area)
                    ->conf('page', 'index')
                    ->conf('method', $method)
                    ->conf('ext', octoms::$oms_info['ext'])
                    ->conf('template', (is_null($general_template) || !file_exists(APP_TPL . DS . $general_template . DS . $area)) ? FERV_TEMPLATE : $general_template)
                    ->css(array('style'))
                    ->meta('logo', baseurl('favicon.ico'));

                // Run pre-controller code
                $this->_pre();

                // Set the search description XML link
                $this->template->meta(
                    'search',
                    array(
                        'href' => i18n_baseurl('search/open/description.xml'),
                        'title' => t(
                            __FILE__,
                            5,
                            '%1 - Search (%2)',
                            $this->settings->get('general.site_name'),
                            lang()
                        )
                    )
                );

                // Initialize the addon
                $controller = new $area;
                $controller->model = (class_exists($model = $area . '_m') ? new $model : false);

                // Bind all autoloaded objects to this addon
                if (!empty(octoms::$oms_objects)) {
                    foreach (octoms::$oms_objects as $key => $obj) {
                        $key = substr($key, 0, strrpos($key, '_'));
                        $this->$key = $obj;
                        $controller->$key = $obj;
                        if ($controller->model !== false) {
                            $controller->model->$key = $obj;
                        }
                    }
                }

                // Call the _pre method
                if (method_exists($controller, '_pre') && $this->allowMethod) {
                    call_user_func_array(array($controller, '_pre'), $args);
                }

                // Call the method
                if (method_exists($controller, $method) && $this->allowMethod) {
                    // Set the breadcrumbs
                    if ($this->template->conf('area') == 'admin') {
                        $this->template->breadcrumbs(
                            array('Home', 'href' => i18n_baseurl()),
                            array('Admin', 'href' => i18n_baseurl('admin')),
                            array(
                                ucfirst(strtolower($this->template->conf('addon'))),
                                'href' => i18n_baseurl(
                                    'admin/' . strtolower($this->template->conf('addon'))
                                )
                            ),
                            array(
                                ucfirst($this->template->conf('method')),
                                'href' => i18n_baseurl(
                                    'admin/' . strtolower($this->template->conf('addon')) .
                                    '/' . $this->template->conf('method')
                                )
                            )
                        );
                        if (isset($args[0]))
                            $this->template->app(
                                'breadcrumbs',
                                array(
                                    ucfirst($args[0]),
                                    'href' => i18n_baseurl(
                                        'admin/' . strtolower($this->template->conf('addon')) . '/' .
                                        $this->template->conf('method') . '/' .
                                        strtolower($args[0])
                                    )
                                )
                            );
                    } else {
                        $this->template->breadcrumbs(
                                array('Home', 'href' => $this->template->url()),
                                array(isset($args[0]) ? ucfirst($args[0]) : 'Page')
                        );
                    }

                    // Call the method
                    call_user_func_array(array($controller, $method), $args);
                } else {
                    // Maybe the index method is not allowed. Find the next one
                    if (
                        isset($this->addonMethods[$this->template->conf('addon')]) &&
                        count($this->addonMethods[$this->template->conf('addon')]) > 0
                    ) {
                        // Back to the defaults
                        octoms::$oms_info['stage'] = 'dispatch';
                        octoms::$oms_info['controller'] = 'dispatch';
                        octoms::$oms_info['method'] = __FUNCTION__;
                        octoms::$oms_info['parameters'] = func_get_args();

                        // Redirect to the new location
                        $this->template->redirect(
                            $this->template->conf('area') . WS . $this->template->conf('addon') . WS .
                            @array_shift(
                                array_keys(
                                    $this->addonMethods[$this->template->conf('addon')]
                                )
                            )
                        );
                    }
                    // Or we are here by accident
                    else {
                        $this->error->page(404);
                    }
                }

                // Populate the regions
                !is_null($regions = $this->settings->getChildren(
                    sprintf(
                        'regions.%s',
                        $this->template->conf('page')
                    )
                )) ?
                $this->template->region($regions) :
                null;

                // Call the _post method
                if (method_exists($controller, '_post') && $this->allowMethod)
                    call_user_func_array(array($controller, '_post'), $args);

                // Any final operations?
                $this->_post();

                // Load the template
                $this->template->flush();
            }
            // Controller not found
            else {
                $this->error->page(404);
            }
        }
        // Addon not found
        else {
            $this->error->page(404);
        }
    }// end function __call()

    /**
     * Get available methods for the given addon
     * 
     * @param string $addon
     * @return array $this->addonMethods
     */
    protected function _methods($addon = '') {
        // Return the methods if they were already discovered
        if (isset($this->addonMethods[$addon]))
            return $this->addonMethods[$addon];

        // Get all the class methods
        $r = array();

        // Load the class file as a string
        if (false !== $class = file_get_contents(STAGES . DS . 'addons' . DS . $addon . DS . 'controllers' . DS . $this->template->conf('area') . EXT)) {
            // Find all comment areas
            preg_match_all('%/\*\*([^\e]*?)\*/[^\e]*?(function[\s]+.*?)+?\(%i', $class, $reg);

            // Add user information
            if (isset($reg[1])) {
                foreach ($reg[1] as $k => $v) {
                    // Remove the "function" precursor
                    $reg[2][$k] = trim(str_replace('function', '', $reg[2][$k]));

                    // Do not count hidden methods
                    if (strpos($reg[2][$k], '_') !== 0) {
                        // Full info on the current addon only
                        if ($addon == $this->template->conf('addon')) {
                            // Prepare the method information
                            $r[$reg[2][$k]] = array(
                                'name' => ucfirst($reg[2][$k]),
                                'description' => t(__FILE__, 3, 'The %1 method', $reg[2][$k]),
                                'author' => t(__FILE__, 4, 'Fervoare CMS Team'),
                                'version' => '0.0.1',
                                'allow' => 'admin',
                                'selected' => ($reg[2][$k] == $this->template->conf('method'))
                            );

                            // Remove the *
                            $v = preg_replace('/.*\*([^\n].*\n)/i', '$1', $v);

                            // Get the @ specifications
                            preg_match_all('/@([\w]+)\b([^@]+\n?)/i', $v, $a);
                            $lang = lang();
                            if (count($a[1]) > 0) {
                                $roles = array_keys($this->auth->roles);
                                foreach ($a[1] as $_k => $_v) {
                                    $_v = trim($_v);
                                    if (!in_array($_v,
                                        array(
                                            'param',
                                            'return',
                                            'link',
                                            'example',
                                            'package',
                                            'ignore'
                                        )
                                    )) {
                                        // Use only one of the defined roles
                                        if ($_v == 'allow' && !in_array(trim($a[2][$_k]), $roles)) {
                                            $a[2][$_k] = 'admin';
                                        }

                                        // Append the information
                                        $r[$reg[2][$k]][$_v] = trim($a[2][$_k]);
                                    } elseif ($_v == $lang && $_v != DEFAULT_LANGUAGE) {
                                        // Append the information
                                        $r[$reg[2][$k]][$_v] = trim($a[2][$_k]);
                                    }
                                }
                            }

                            // Get the description and name
                            if (isset($r[$reg[2][$k]][$lang])) {
                                $desc = explode(':', trim($r[$reg[2][$k]][$lang]));
                                unset($r[$reg[2][$k]][$lang]);
                            } else {
                                $desc = explode(':',
                                    trim(preg_replace('/@([\w]+)\b([^@]+\n?)/i', '', $v)
                                ));
                            }

                            // Update the values
                            $r[$reg[2][$k]]['name'] = array_shift($desc);
                            count($desc) > 0 ? 
                                $r[$reg[2][$k]]['description'] = trim(implode(':', (array) $desc))
                                : null;

                            // Is this method not allowed?
                            if ($this->auth->role() > $this->auth->roles[$r[$reg[2][$k]]['allow']]) {
                                // Remove it from the list
                                unset($r[$reg[2][$k]]);

                                // Set the flag if the user tries to access it anyway
                                if ($addon == $this->template->conf('addon') && $reg[2][$k] == $this->template->conf('method')) {
                                    $this->allowMethod = false;
                                }
                            }
                        } else {
                            // Remove the *
                            $v = preg_replace('/.*\*([^\n].*\n)/i', '$1', $v);

                            // Get the @ specifications
                            preg_match_all('/@([\w]+)\b([^@]+\n?)/i', $v, $a);
                            $role = 'admin';
                            if (count($a[1]) > 0) {
                                $roles = array_keys($this->auth->roles);

                                foreach ($a[1] as $_k => $_v) {
                                    // Use only one of the defined roles
                                    if ($_v == 'allow') {
                                        $role = !in_array(trim($a[2][$_k]), $roles) ? 'admin' : trim($a[2][$_k]);
                                        continue;
                                    }
                                }
                            }
                            if ($this->auth->role() <= $this->auth->roles[$role]) {
                                $r[] = $reg[2][$k];
                            }
                        }
                    }
                }
            }
        }

        // Set the methods
        return $this->addonMethods[$addon] = $r;
    }// end function _methods()

    /**
     * Post operations - Called after the method
     */
    protected function _post() {
        // Listen for the keyword #*# for the AJAX interface
        if ($this->input->post('#*#')) {
            // Prepare the API response array
            $api = array();

            // Set the status
            is_null($api['_status_'] = $this->template->getOne('api_status')) ?
                ($api['_status_'] = array('Done.', true)) :
                null;

            // Set the filters
            $api['_filters_'] = (is_null($api['_filters_'] = $this->template->getOne('api_filters'))) ?
                array() :
                array_flat($api['_filters_']);

            // Set the content
            !is_null($cnt = $this->template->getOne('api_content')) ? ($api['_content_'] = $cnt) : null;

            // This is a plain JSON response
            $this->template->conf('whitescreen', true);
            ob_end_clean();

            // We <3 JSON
            print json_encode($api);
        } else {
            // Add information to the publicly visible content
            if ($this->template->conf('area') == 'open') {
                // Add some default meta information
                $meta = $this->template->getOne('meta');

                // No description?
                !isset($meta['description']) ?
                    ($meta['description'] = $this->input->abstr(
                    $this->input->clean_all(ob_get_contents()), 100)
                    ) :
                    null;

                // Save the changes
                $this->template->set('meta', $meta);

                // Plaintext?
                if ($this->template->conf('ext') == 'txt') {
                    // Send a plain/text header
                    header('Content-type:text/plain');

                    // Get the content
                    $content = ob_get_clean();

                    // Add the title
                    $content = '==' . $this->template->getOne('title') . "==\n\n----\n" . $content;

                    // Add the keywords
                    if (isset($meta['keywords'])) {
                        $content .= "\n----\n" . implode(', ', $meta['keywords']);
                    }

                    // Add the powered by
                    $content .= "\n\n" . sprintf(
                        'Generated by the Fervoare CMS for Startups in %ss @%sMB.',
                        round(microtime(true) - octoms::$oms_info['start'], 3),
                        round(memory_get_usage() / 1048576, 2)
                    );

                    // Clean the content
                    $content = $this->input->clean_article($content, false);

                    // Do not load any templates
                    $this->template->conf('whitescreen', true);

                    // Print the content
                    print wordwrap($content);
                    exit();
                } elseif ($this->template->conf('ext') == 'xml') {
                    // Send an xml header
                    header('Content-type:text/xml');

                    // Get the content
                    $content = ob_get_clean();

                    // Do not load any templates
                    $this->template->conf('whitescreen', true);

                    // Prepare the data
                    $data = array(
                        'page' => array(
                            'title' => $this->template->getOne('title'),
                            'content' => wordwrap($this->input->clean_article($content, false))
                        ),
                        sprintf(
                                'generator:execution=%s,memory=%s',
                                round(microtime(true) - octoms::$oms_info['start'], 3) . ' s',
                                round(memory_get_usage() / 1048576, 3) . ' MB'
                        ) => array(
                            'name' => 'Fervoare CMS for Startups',
                            'version' => FERV_VERSION
                        ),
                        'publisher' => 'S.C. Fervoare CMS'
                    );

                    // Add keywords?
                    if (isset($meta['keywords'])) {
                        $data['page']['keywords'] = implode(', ', $meta['keywords']);
                    }

                    // Print the XML
                    print array_to_xml($data, 'about');
                    exit();
                }

                // Google Analytics
                if (!is_null($ga_id = $this->settings->get('seo.ga_id')) && !empty($ga_id)) {
                    $this->template->js(
                        array(
                            'remote' => false,
                            sprintf(
                                "var _gaq = _gaq || [];\n" .
                                "_gaq.push(['_setAccount', '%s']);\n" .
                                "_gaq.push(['_trackPageview']);\n" .
                                "(function() {\n" .
                                "    var ga = document.createElement('script'); \n" .
                                "    ga.type = 'text/javascript'; \n" .
                                "    ga.async = true;\n" .
                                "    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';\n" .
                                "    var s = document.getElementsByTagName('script')[0]; \n" .
                                "    s.parentNode.insertBefore(ga, s);\n" .
                                "})();",
                                $ga_id
                            )
                        )
                    );
                }

                // Customize the title
                if (
                    !in_array(
                        $this->template->conf('page'),
                        array(
                            'index',
                            'blog_index'
                        )
                    ) 
                    && !is_null($site_name = $this->settings->get('general.site_name')) 
                    && !empty($site_name)
                ) {
                    $this->template->conf(
                        'title',
                        $site_name . ' - ' . $this->template->conf('title')
                    );
                }
            }

            // Append extra JavaScript and CSS as defined in the theme's info.ini
            if (null !== $settings = $this->settings->get(
                    'general.template.' . $this->template->conf('template') . '.settings'
                    )
            ) {
                foreach (array('js', 'css') as $el) {
                    if (isset(
                        $settings[
                            $this->template->conf('area') . '_' .
                            $this->template->conf('page') . '_' . $el
                        ]
                    )) {
                        $data = array('custom' => true);
                        foreach (
                            $settings[
                                $this->template->conf('area') . '_' .
                                $this->template->conf('page') . '_' .
                                $el
                            ] as $script
                        ) {
                            if (false !== strpos($script, '//')) {
                                $data[] = $script;
                            } else {
                                $data[] = baseurl(
                                    'app/templates/' .
                                    $this->template->conf('template') . '/' .
                                    $this->template->conf('area') . '/' .
                                    $el . '/' . $script
                                );
                            }
                        }
                        call_user_func(array($this->template, $el), $data);
                    }
                }
            }

            // Load the init functions
            $this->template->jQueryInit();
        }
    }// end function _post()
}

/* End Of File <dispatch.inc> */