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
    
class template_l {
    /**
     * Page data
     */
    public $data;

    /**
     * Try catch procedure bug
     */
    public $bug = array('status' => true, 'message', 'code');

    /**
     * Prevent errors if the user calls an invalid method
     * 
     * @param string $method
     * @param array $args
     * @return null
     */
    public function __call($method = '', $args = array()) {
        // Don't return an error if the user calls the wrong function
        return;
    }

    /**
     * Don't display errors if the user accidentally tries to prin $this
     * 
     * @return null
     * 
     */
    public function __toString() {
        return '';
    }

    /**
     * Load the template, set the content and exit
     * 
     * @return void
     */
    public function flush() {
        // Don't display the template if the user requested it
        if ($this->conf('whitescreen') == true) {
            return;
        }

        // Set the content
        $this->set('content', ob_get_clean());

        // Load the template file
        if (file_exists($f = FERV_ADDONS . DS . $this->conf('addon') . DS . 'views' . DS . $this->conf('area') . DS . $this->conf('page') . '.tpl')) {
            require_once $f;
        } elseif (file_exists($f = APP_TPL . DS . $this->conf('template') . DS . $this->conf('area') . DS . $this->conf('page') . '.tpl')) {
            require_once $f;
        } elseif (file_exists($f = APP_TPL . DS . FERV_TEMPLATE . DS . $this->conf('area') . DS . $this->conf('page') . '.tpl')) {
            require_once $f;
        } else {
            throw new Exception('Tried to load invalid template file "' . $f . '".');
        }

        // Nothing beyond this point
        exit;
    }

    /**
     * Fervoare Admin API
     * 
     * @example 
     * // Set the status message
     * {template}->api('Update completed.');
     * // Set an error status message (appended)
     * {template}->api('Update failed.',false);
     * // Replace previously set status messages
     * {template}->api('Final status.',false,false);
     * // Set the list of filters to apply to the content
     * {template}->api(array('f1','f2'));
     * // Replace previously set filters
     * {template}->api(array('f1','f2'),false);
     * 
     * @param mixed $a - array for filters, string or boolean for status msg.
     * @param boolean $b - type of status message/append the filters?
     * @param boolean $append - append the message?
     * @return template_l
     */
    public function api($a = null, $b = true, $append = true) {
        // The first argument is mandatory
        if (empty($a))
            return $this;

        // Set the status message
        if (is_string($a)) {
            // Status already set?
            if (is_null($status = $this->getOne('api_status')) || !$append) {
                // Set the status the first time
                $this->set(
                    'api_status',
                    array(
                        $a,
                        $b
                    )
                );
            } else {
                // Append to the status message and change the status type
                $this->set(
                    'api_status',
                    array(
                        $status[0] . ' ' . $a,
                        $status[1] && $b
                    )
                );
            }
        }
        // Set the filters
        elseif (is_array($a)) {
            // Filters already set?
            if (is_null($filters = $this->getOne('api_filters')) || !$b) {
                // Set the filters for the first time
                $this->set(
                    'api_filters',
                    $a
                );
            } else {
                // Append to the filters list
                $this->set(
                    'api_filters',
                    array_merge($filters, $a)
                );
            }
        }

        // Return self
        return $this;
    }

    /**
     * Set or get the API request content
     * 
     * @example 
     * // The _content_ parameter will be populated
     * {template}->api_cnt('a new page element');
     * // Get the data sent as the API content
     * {template}->api_cnt();
     * 
     * @param string $str
     * @return template_l|mixed - currently set value or null if not set
     */
    public function api_cnt($str = null) {
        // Return the content?
        if (is_null($str)) {
            return $this->getOne('api_content');
        } else {
            // Content already set?
            if (is_null($content = $this->getOne('api_content'))) {
                // Set the content for the first time.
                return $this->set(
                                'api_content',
                                $str
                );
            } else {
                // Array?
                if (is_array($content) || is_object($content)) {
                    // Make sure it is an array
                    $content = object_to_array($content);

                    // Append the content
                    $content = array_merge($content, (array) $str);
                } else {
                    if (is_array($str)) {
                        $content = array_merge((array) $content, $str);
                    } else {
                        $content .= $str;
                    }
                }

                // Append content to the result; no spaces;
                return $this->set(
                    'api_content',
                    $content
                );
            }
        }
    }

    /**
     * Append; data storage
     * 
     * @example 
     * // Append $x to the 'y' key
     * {template}->app('y',$x);
     * 
     * @param string $key
     * @param mixed $value
     * @return template_l
     */
    public function app($key = '', $value = null) {
        if ($key == '__conf__') {
            return $this;
        }

        // Allow any number of values to be sent
        $values = func_get_args();
        array_shift($values);
        if (count($values) == 0)
            $values = array(null);

        foreach ($values as $value) {
            if (!isset($this->data[$key]) && strlen($key) > 0) {
                $this->set($key, $value);
            } elseif (strlen($key) > 0) {
                $get = $this->get($key);
                $get[] = $value;
                $this->data[$key] = $get;
            }
        }
        return $this;
    }

    /**
     * Try-catch procedure; Exception
     * 
     * @example 
     * // Issue a template exception (catchable by trap trap/snap functions)
     * {template}->bug('Some exception',10);
     * 
     * @param string $message
     * @param string|int $code
     * @return false
     */
    public function bug($message = '', $code = '') {
        $this->bug = array('status' => false, 'message' => $message, 'code' => $code);
        return false;
    }

    /**
     * Clean; data storage
     * 
     * @example 
     * // Remove the 'abc' key
     * {template}->clean('abc');
     * 
     * @param string $key
     * @return template_l
     */
    public function clean($key = '') {
        if (!empty($key) && isset($this->data[$key]) && $key != '__conf__') {
            unset($this->data[$key]);
        }
        return $this;
    }

    /**
     * Set or get configuration parameters; 
     * allowed values are [whitescreen, template, method, addon, area, page]
     * 
     * @example 
     * // Set the page
     * {template}->conf('page','pagex');
     * // Get the page
     * {template}->conf('page');
     * 
     * @param string $key
     * @param mixed $value
     * @return template_l|mixed-the value
     */
    public function conf($key = null) {
        // Special config keys
        if (!in_array($key, array('whitescreen', 'template', 'method', 'addon', 'area', 'page', 'ext'))) {
            return false;
        }

        // Function arguments
        $args = func_get_args();

        // Set a value
        if (isset($args[1])) {
            $this->data['__conf__'][$key] = $args[1];
            return $this;
        }
        // Retrieve a value
        else {
            return isset($this->data['__conf__'][$key]) ? $this->data['__conf__'][$key] : null;
        }
    }

    /**
     * Get; data storage
     * 
     * @example 
     * // Get the contents of the 'y' key
     * {template}->get('y')
     * 
     * @param string $key
     * @return array of mixed values / null if not found
     */
    public function get($key = '') {
        if (strlen($key) > 0 && $key != '__conf__') {
            return isset($this->data[$key]) ? $this->data[$key] : null;
        } else {
            return null;
        }
    }

    /**
     * Get 1 value; data storage
     * 
     * @example 
     * // Instead of getting array(0=>"data") you get "data"
     * {template}->getOne('y'); # see {template}->get()
     * 
     * @param string $key
     * @return mixed|null
     * 
     */
    public function getOne($key = '') {
        return (is_null($r = $this->get($key)) ? null : $r[0]);
    }

    /**
     * Add a jQuery module as defined in ADDONS/{this addon}/views/{admin or open}/jquery.ini
     * The INI file looks like this
     * [ckeditor]
     * js[] = "ckeditor.js" ;this is found in /js/ckeditor.js
     * css[] = "ckstyle.css" ;this is found in /css/ckstyle.css
     * init = "ckeditor_init.js" ; this file is loaded onpage in $(document).ready()
     * 
     * @example 
     * // Add the ckeditor functionality
     * {template}->jQuery('ckeditor');
     * # This will add the javascript files found in the /js folder
     * # and the CSS files found in the /css folder
     * # and load the "ckeditor_init.js" file in the /js folder
     * # in a $(document).ready() onpage
     * 
     * // Pass the $a='text'; variable
     * {template}->jQuery('ckeditor',null,null,array('a'=>'text'));
     * # This may be used as {a} (a between brackets) in the script
     * # / * before {a} after * / - 'before text after' 
     * # Already defined (without spaces):
     * # / * {INIT} * / - the web address of this _init.js script's folder
     * # / * {HERE} * / - the root address of the current page
     * # / * {COOKIE} * / - the current cookie
     * # / * {HTTP_USER_AGENT} * / - the current user agent
     * 
     * 
     * @param string|array $func
     * @param string $addon
     * @param string $area - admin or open
     * @param array $args - optional arguments to pass to the {}_init.js file
     * @return template_l
     */
    public function jQuery($func = null, $addon = null, $area = null, $args = array()) {
        if (file_exists($f = FERV_ADDONS . DS . (!is_null($addon) ? $addon : $this->conf('addon')) . DS . 'views' . DS . (!is_null($area) ? $area : $this->conf('area')) . DS . 'jquery.ini')) {
            if (false !== $info = parse_ini_file($f, true)) {
                $func = is_array($func) ? $func : (array) $func;
                $init = '';
                foreach ($func as $f) {
                    if (isset($info[$f])) {
                        // Add the JavaScript
                        isset($info[$f]['js']) ? $this->js(array_merge((array) $info[$f]['js'], array('tpl' => false, 'addon' => $addon, 'area' => $area))) : null;
                        // Add the CSS
                        isset($info[$f]['css']) ? $this->css(array_merge((array) $info[$f]['css'], array('tpl' => false, 'addon' => $addon, 'area' => $area))) : null;
                        // Append the init JavaScript
                        if (isset($info[$f]['init']) && file_exists($i = FERV_ADDONS . DS . (!is_null($addon) ? $addon : $this->conf('addon')) . DS . 'views' . DS . (!is_null($area) ? $area : $this->conf('area')) . DS . 'js' . DS . $info[$f]['init'])) {
                            $args['INIT'] = $this->url(null, false, $addon, $area);
                            $args['HERE'] = here();
                            $args['COOKIE'] = isset($_COOKIE[COOKIE_NAME]) ? $_COOKIE[COOKIE_NAME] : '';
                            $args['HTTP_USER_AGENT'] = isset($_SERVER['HTTP_USER_AGENT']) ? $_SERVER['HTTP_USER_AGENT'] : '';
                            if (false !== $ic = file_get_contents($i)) {
                                foreach ($args as $k => $v) {
                                    $ic = preg_replace('%/\*(.*?)\{' . $k . '\}(.*?)\*/%i', '${1}' . $v . '$2', $ic);
                                }
                                $init .= $ic;
                            }
                        }
                    }
                }
                // Append the init string
                is_null($__jq__ = $this->getOne('__jq__')) ? $this->set('__jq__', $init) : $this->set('__jq__', $init . "\n" . $__jq__);
            }
        } else {
            throw new Exception('Invalid configuration file specified "' . $f . '".');
        }

        return $this;
    }

    /**
     * Load the jQuery models init functions
     * 
     * @return template_l
     */
    public function jQueryInit() {
        // Only load the document ready declaration if there are scripts to encapsulate
        if (!is_null($js = $this->getOne('__jq__')))
            $this->js(array(sprintf("$(document).ready(function(){\n%s\n});", $js), 'remote' => false));

        // Done
        return $this;
    }

    /**
     * Redirect template function
     * 
     * @example 
     * // Set a Location header
     * {template}->redirect('admin')
     * 
     * @param string $page
     * @return void
     */
    public function redirect($page = '') {
        if (!debugging() || ENVIRONMENT) {
            @ob_end_clean();
            @header(
                'location:' . str_replace(
                    array(
                        "\r\n",
                        "\r",
                        "\n"
                    ),
                    '',
                    i18n_baseurl($page)
                )
            );
        } else {
            header('octoms-redirect:' . i18n_baseurl($page));
        }
        exit();
    }

    /**
     * Set; data storage
     * 
     * @example 
     * // Set the 'y' key to $x
     * {template}->set('y',$x);
     * 
     * @param string $key
     * @param mixed $value
     * @return template_l
     * 
     */
    public function set($key = '', $value = null) {
        if (strlen($key) > 0 && $key != '__conf__') {
            $this->data[$key] = array($value);
        }
        return $this;
    }

    /**
     * Shortcodes implementation
     * 
     * @example 
     * // Load the 'news' widget
     * {template}->shortcode('{widget=news}');
     * // Load the 'x' method of the 'news' widget
     * {template}->shortcode('{widget=news:x}');
     * // Load the 'x' method of the 'news' widget with params 'a' and 'b'
     * {template}->shortcode('{widget=news:x:a,b}');
     * 
     * @param string $str
     * @return string - the result of the shortcode
     * 
     */
    public function shortcode($str = '') {
        // Perform the replace
        return preg_replace_callback('/\{([^=]+)=([^}]*)\}/i', array($this, '_shortcode_helper'), $str);
    }

    /**
     * Shortcode helper
     */
    protected function _shortcode_helper($m) {
        // Allowed operations
        $allowed = array('widget');

        if (in_array($m[1], $allowed) && method_exists($this, $m[1])) {
            if ($m[1] == "widget") {
                $exp = explode(":", $m[2]);
                ob_start();
                call_user_func_array(
                    array(
                        $this,
                        'widget'
                    ),
                    array(
                        isset($exp[0]) ? $exp[0] : null,
                        isset($exp[1]) ? $exp[1] : null,
                        isset($exp[2]) ? explode(",", $exp[2]) : null
                    )
                );
                return ob_get_clean();
            }
        }

        return $m[0];
    }

    /**
     * Breadcrumbs
     * 
     * @example 
     * // Display the breadcrumbs
     * {template}->breadcrumbs();
     * // Append 1 or more breadcrumbs
     * {template}->breadcrumbs(array('Link to page x', 'href'=>'http://x.example', 'options'=>array())[,array()]);
     * 
     * @param array $breadcrumb
     * @return template_l
     */
    public function breadcrumbs() {
        // This is the separation character
        $char = '&rarr;';

        // Use this as an append function
        if (count($args = func_get_args()) > 0) {
            // Change the separation character
            if (is_string($args[0])) {
                $char = $args[0];
            }
            // Append a breadcrumb
            else {
                array_unshift($args, 'breadcrumbs');
                return call_user_func_array(array($this, 'app'), $args);
            }
        }

        // Use this as a view tool
        $breadcrumbs = $this->get('breadcrumbs');
        if (!empty($breadcrumbs) && is_array($breadcrumbs)) {
            $r = array();
            foreach ($breadcrumbs as $b) {
                $r[] = a(isset($b[0]) ? $b[0] : '[link]', isset($b['href']) ? $b['href'] : null, isset($b['options']) ? $b['options'] : null);
            }
            print implode($char, $r);
        }

        return $this;
    }

    /**
     * Display the page's content
     * 
     * @example 
     * // Display the page content
     * {template}->content();
     * 
     * @param string $content
     * @return template_l
     * 
     */
    public function content() {
        print $this->getOne('content');
        return $this;
    }

    /**
     * Set or display the page's CSS scripts
     * 
     * @example 
     * // Display all of the CSS
     * {template}->css();
     * // Append a line of CSS
     * {template}->css(array('remote'=>false,'body {display: block;}'));
     * // Append a remote CSS file from the template
     * {template}->css(array('script'));
     * {template}->css(array('script.css'));
     * // Append a remote CSS file from the addon views
     * {template}->css(array('script','tpl'=>false));
     * // Append multiple CSSs
     * {template}->css(array('script1'[,script2][,'remote'=>true/false][,'tpl'=>true/false])[,array()]);
     * 
     * @param array $css
     * @return template_l
     */
    public function css() {
        // Use this as an append function
        if (count($args = func_get_args()) > 0) {
            array_unshift($args, 'css');
            return call_user_func_array(array($this, 'app'), $args);
        }

        // Use this as a view tool
        $css = $this->get('css');
        if (!empty($css)) {
            foreach ($css as $v) {
                if (is_array($v) && count($v) > 0) {
                    foreach ($v as $_k => $_v) {
                        if (is_numeric($_k) && !empty($_v)) {
                            print css(
                                    (isset($v['remote']) && $v['remote'] === false) ?
                                        $_v :
                                        $this->url(
                                            (isset($v['custom']) || strpos($_v, '//') !== false) ? $_v : ('css/' . $_v . (strpos($_v, '.css') !== false ? '' : '.css')),
                                            isset($v['tpl']) ? $v['tpl'] : null,
                                            isset($v['addon']) ? $v['addon'] : null,
                                            isset($v['area']) ? $v['area'] : null,
                                            isset($v['custom']) ? $v['custom'] : null
                                        ),
                                    null,
                                    isset($v['remote']) ? $v['remote'] : true
                            ) . "\n\t";
                        }
                    }
                } elseif (is_string($v)) {
                    print css($this->url('css/' . $v . (strpos($v, '.css') !== false ? '' : '.css'))) . "\n\t";
                }
            }
        }

        return $this;
    }

    /**
     * Try-catch procedure; end trap
     * 
     * @example see 'template trap'
     * 
     * @return void
     * 
     */
    public function endtrap() {
        $this->bug['status'] === true ? (print ob_get_clean()) : ob_clean();
    }

    /**
     * Google Analytics script
     * 
     * @param string $id - Google Analytics ID
     * @param array $custom - Associative array of custom variables; maximum size of 4
     * @return null
     * 
     */
    public function ga($id = null, $custom = array('domain' => DOMAIN)) {
        // The ID is a must            
        if (!is_null($id)) {
            $format = '_gaq.push([\'_setCustomVar\', %s, \'%s\', \'%s\', 3]);' . "\n";
            $customVar = '';
            if (is_array($custom) && count($custom) > 0 && array_is_assoc($custom)) {
                $i = 1;
                foreach ($custom as $key => $value) {
                    $customVar .= sprintf(
                            $format,
                            $i,
                            htmlspecialchars($key, ENT_QUOTES),
                            htmlspecialchars($value, ENT_QUOTES)
                    );
                    if ($i == 4)
                        break;
                    $i++;
                }
                $customVar .= sprintf($format, 5, 'domain', DOMAIN);
            }

            print '<script type="text/javascript">
                var _gaq = _gaq || [];
                _gaq.push([\'_setAccount\', \'' . $id . '\']);
                _gaq.push([\'_setDomainName\', \'' . DOMAIN . '\']);
                _gaq.push([\'_setAllowLinker\', true]);
                ' . $customVar . '
                _gaq.push([\'_trackPageview\',\'' . DOMAIN . $_SERVER['REQUEST_URI'] . '\']);
                (function() {
                    var ga = document.createElement(\'script\');
                    ga.type = \'text/javascript\';
                    ga.async = true;
                    ga.src = (\'https:\' == document.location.protocol ? \'https://ssl\' : \'http://www\') + \'.google-analytics.com/ga.js\';
                    var s = document.getElementsByTagName(\'script\')[0];
                    s.parentNode.insertBefore(ga, s);
                })();
                </script>';
        }
    }

    /**
     * Display the HEAD scripts and information
     * 
     * @example 
     * // Load the JS,CSS,Meta etc.
     * {template}->head();
     * // Also preload the 'w1' widget;
     * {template}->head('w1');
     * //.. or widgets 'w1' and 'w2'
     * {template}->head(array('w1','w2'));
     * {template}->head('w1','w2');
     * 
     * @param string $preload_widget
     * @return template_l
     */
    public function head() {
        // Preload some widget CSS
        if (count($args = func_get_args()) > 0) {
            // Use the first argument only
            if (count($args) == 1)
                $args = (array) $args[0];

            // Load the widget
            foreach ($args as $widget) {
                $this->widget($widget, true);
            }
        }
        return $this->title(true)->css()->meta()->js();
    }

    /**
     * Set or display the page's JS scripts
     * 
     * @example 
     * // Display all of the JS
     * {template}->js();
     * // Append a line of JS
     * {template}->js(array('remote'=>false,'var x = function(){};'));
     * // Append a remote JS file from the template
     * {template}->js(array('script'));
     * {template}->js(array('script.js'));
     * // Append a remote JS file from the addon views
     * {template}->js(array('script','tpl'=>false));
     * // Append multiple JSs
     * {template}->js(array('script1'[,script2][,'remote'=>true/false][,'tpl'=>true/false])[,array()]);
     * 
     * @param array $js
     * @return template_l
     */
    public function js() {
        // Use this as an append function
        if (count($args = func_get_args()) > 0) {
            array_unshift($args, 'js');
            return call_user_func_array(array($this, 'app'), $args);
        }

        // Use this as a view tool
        $js = $this->get('js');
        if (!empty($js)) {
            foreach ($js as $v) {
                if (is_array($v) && count($v) > 0) {
                    foreach ($v as $_k => $_v) {
                        if (is_numeric($_k) && !empty($_v)) {
                            print js(
                                (isset($v['remote']) && $v['remote'] === false) ?
                                    $_v :
                                    $this->url(
                                        (isset($v['custom']) || strpos($_v, '//') !== false) ? $_v : ('js/' . $_v . (strpos($_v, '.js') !== false ? '' : '.js')),
                                        isset($v['tpl']) ? $v['tpl'] : null,
                                        isset($v['addon']) ? $v['addon'] : null,
                                        isset($v['area']) ? $v['area'] : null,
                                        isset($v['custom']) ? $v['custom'] : null
                                    ),
                                isset($v['remote']) ? $v['remote'] : true
                            ) . "\n\t";
                        }
                    }
                } elseif (is_string($v)) {
                    print js($this->url('js/' . $v . (strpos($v, '.js') !== false ? '' : '.js'))) . "\n\t";
                }
            }
        }

        return $this;
    }

    /**
     * Append or display the menu
     * 
     * @example 
     * // Display the 'default' menu
     * {template}->menu();
     * // Display the 'foo' menu
     * {template}->menu('foo');
     * // Display the 'foo' menu with class 'bar'
     * {template}->menu('foo',array('class'=>'bar'));
     * // Append a 'baz' menu
     * {template}->menu('baz',$menu_structure,true);
     * 
     * @param array $menu
     * @return template_l
     */
    public function menu($name = null, $options = null, $add = false) {
        // Use this as an append function
        if ($add) {
            // Get the menus
            $menus = $this->getOne('menus');

            // Append or replace the menu
            if (is_null($menus)) {
                $menus = array($name => $options);
            } else {
                $menus[$name] = $options;
            }

            // Save it
            $this->set('menus', $menus);

            // All done
            return $this;
        }
        // Use this as a view tool
        else {
            // Get the menus
            $menus = $this->getOne('menus');

            // Use the default?
            if (is_null($name))
                $name = 'default';

            // Print the one requested
            if (is_string($name) && isset($menus[$name])) {
                print l(
                                $menus[$name],
                                null,
                                $options
                );
            }

            // Return this
            return $this;
        }
    }

    /**
     * Set or display specific meta information keys: <ul>
     * <li>description</li>
     * <li>keywords</li>
     * <li>logo</li>
     * <li>charset</li>
     * <li>author</li>
     * <li>copyright</li>
     * <li>robots</li>
     * <li>search</li>
     * </ul>
     * 
     * @example 
     * // Display the meta information
     * {template}->meta();
     * // Set the keywords
     * {template}->meta(array('keywords'=>array('a','b'));
     * {template}->meta(array('keywords'=>'a,b,'));
     * 
     * @param array $info - meta information
     * @return template_l
     */
    public function meta($info = null) {
        // Get the available meta information
        $meta = (array) $this->getOne('meta');

        // Print the meta
        if (is_null($info)) {
            // Prepare the string
            $str = array();

            // Add the logo
            isset($meta['logo']) ? $str[] = tag('link', array('rel' => 'icon', 'type' => 'image/x-icon', 'href' => $meta['logo']), null, true) : null;

            // Add the Search XML reference
            isset($meta['search']) ? $str[] = tag('link', array('rel' => 'search', 'type' => 'application/opensearchdescription+xml', 'href' => $meta['search']['href'], 'title' => $meta['search']['title']), null, true) : null;

            // Print version
//                $str[] = tag('link',array('rel'=>'alternate','type'=>'text/txt','href'=>here().'.txt','media'=>'print'),null,true);
            // About page
//                $str[] = tag('link',array('rel'=>'alternate','type'=>'application/xhtml+xml','href'=>here().'.xml','media'=>'about'),null,true);
            // Set the charset and content type
            $str[] = tag('meta', array('http-equiv' => 'Content-Type', 'content' => (isset($meta['type']) ? $meta['type'] : 'text/html') . '; charset=' . (isset($meta['charset']) ? $meta['charset'] : 'utf-8')), null, true);

            // Add the language
            $str[] = tag('meta', array('http-equiv' => 'Content-Language', 'content' => lang()), null, true);

            // Set the IE Fix
            $str[] = '<!--[if lte IE 10 ]>' . tag('meta', array('http-equiv' => 'X-UA-Compatible', 'content' => 'IE=edge,chrome=1'), null, true) . '<![endif]-->';

            // Add the generator 
            $str[] = tag('meta', array('name' => 'Generator', 'content' => 'Fervoare CMS v.' . FERV_VERSION . ' https://github.com/Stephino/fervoare'), null, true);

            // Add the publisher
            $str[] = tag('meta', array('name' => 'Publisher', 'content' => 'Fervoare CMS'), null, true);

            // Add the copyright
            isset($meta['copyright']) ? $str[] = tag('meta', array('name' => 'Copyright', 'content' => $meta['copyright']), null, true) : null;

            // Add the keywords
            isset($meta['keywords']) ? $str[] = tag('meta', array('name' => 'Keywords', 'content' => implode(', ', $meta['keywords'])), null, true) : null;

            // Add the description
            (isset($meta['description']) && strlen($meta['description']) > 0) ? $str[] = tag('meta', array('name' => 'Description', 'content' => $meta['description']), null, true) : null;

            // Add the robots
            isset($meta['robots']) ? $str[] = tag('meta', array('name' => 'Robots', 'content' => $meta['robots']), null, true) : null;

            // Add modern browser capabilities
            $str[] = tag('meta', array('name' => 'apple-mobile-web-app-capable', 'content' => 'yes'), null, true);
            $str[] = tag('meta', array('name' => 'apple-mobile-web-app-status-bar-style', 'content' => 'black-translucent'), null, true);
            $str[] = tag('meta', array('name' => 'viewport', 'content' => 'initial-scale=1.0, width=device-width'), null, true);

            // Return the string
            print implode("\n\t", $str) . "\n\t";
        }
        // Set it
        else {
            // Get the function arguments
            $args = func_get_args();
            if (count($args) == 2) {
                $meta[$args[0]] = $args[1];
            } elseif (is_array($info)) {
                foreach ($info as $k => $v) {
                    $meta[$k] = $v;
                }
            }

            // Set the meta
            $this->set('meta', $meta);
        }

        return $this;
    }

    /**
     * Display the "powered by" information and page creation statistics
     * 
     * @example $this->powered();
     * 
     * @return template_l
     */
    public function powered() {
        // Please do not modify this
        print 'Powered by ' . a(
            'Fervoare CMS',
            'https://github.com/Stephino/fervoare',
            array(
                'title' => sprintf(
                    'Generated by the Fervoare CMS for Startups in %ss @%sMiB.',
                    round(microtime(true) - octoms::$oms_info['start'], 3),
                    round(memory_get_usage() / 1048576, 3)
                ),
                'target' => '_blank'
            )
        );

        return $this;
    }

    /**
     * Set or display a template region
     * 
     * @example 
     * // Display the region named 'comments'
     * {template}->region('comments');
     * // Set the region named 'comments' with $data
     * {template}->region('comments',$data);
     * // Set multiple regions
     * {template}->regions(array('a'=>'A content','b'=>'B content'));
     * 
     * @throws {template}->bug()
     * 
     * @param string $name
     * @param string|false $content
     * @param boolean|array|string $encapsulate - true for "ferv_{name}" class, array or string for HTML options (like class,id etc.)
     * @return template_l
     */
    public function region($name = '', $content = false, $encapsulate = true) {
        if ($content === false) {
            if (is_string($name)) {
                if (isset($this->data['regions'][$name])) {
                    if ($encapsulate === true) {
                        $encapsulate = 'id="ferv_' . $name . '"';
                    } elseif ($encapsulate !== false) {
                        if (is_array($encapsulate)) {
                            $result = "";
                            foreach ($encapsulate as $k => $v) {
                                $result .= $k . '="' . htmlentities($v, ENT_QUOTES, 'UTF-8') . '" ';
                            }
                            $encapsulate = $result;
                        } elseif (!is_string($encapsulate)) {
                            $encapsulate = '';
                        }
                    }

                    print ($encapsulate !== false ? '<div ' . $encapsulate . '>' : '') . $this->shortcode($this->data['regions'][$name]) . ($encapsulate !== false ? '</div>' : '');
                } else {
                    $this->bug('Region "' . $name . '" not set.');
                }
            } elseif (is_array($name)) {
                foreach ($name as $k => $v) {
                    $this->region($k, $v);
                }
            }
        } elseif (!empty($name)) {
            $this->data['regions'][$name] = $content;
        }

        return $this;
    }

    /**
     * Try-catch procedure; snap - catches (a) _bug() issued within the trap
     * 
     * @example see 'template trap'
     * 
     * @return void
     */
    public function snap() {
        if (!$this->bug['status']) {
            ob_clean();
            $this->bug['status'] = true;
        } else {
            ob_flush();
            $this->bug['status'] = false;
        }
    }

    /**
     * Set or display the page title
     * 
     * @example 
     * // Display the page title as text
     * {template}->title();
     * // Display the page title as a meta title tag
     * {template}->title(true);
     * // Set the page title
     * {template}->title('Awesome page');
     * 
     * @param string $title
     * @return template_l
     */
    public function title() {
        // Use this as an append function
        if (count($args = func_get_args()) > 0 && $args[0] !== true) {
            array_unshift($args, 'title');
            return call_user_func_array(array($this, 'app'), $args);
        }

        // Get the title
        $title = is_null($this->getOne('title')) ? (ucfirst($this->conf('area')) . ' - ' . ucfirst($this->conf('addon')) . ' - ' . ucfirst($this->conf('method'))) : $this->getOne('title');

        // Print it
        print ((isset($args[0]) && $args[0] === true) ? tag('title', null, htmlspecialchars($title)) : $title) . "\n\t";

        return $this;
    }

    /**
     * Try-catch procedure; trap
     * 
     * @example 
     * // Use within a template file; similar to the try{}catch(Exception $e){} block
     * $this->trap();
     * // Try to display the 'test' region
     * # $this->region('test');
     * // Has a _bug() been issued?
     * $this->snap();
     * // Print the template exception message and code
     * # print $this->bug['message'].$this->bug['code'];
     * $this->endtrap();
     */
    public function trap() {
        ob_start();
        $this->bug = array('status' => true, 'message' => '', 'code' => '');
    }

    /**
     * Uploadify!
     * 
     * @example
     * // 1. Set the upload table with id "1"
     * {template}->uploadify(1,'destination');
     * // OR also set some restrictions; limit the upload to 1KB
     * {template}->uploadify(1,'destination',array('sizeLimit'=>1024));
     * // OR also use a custom table structure
     * // see #How the defined method should look like
     * {template}->uploadify(1,'destination',null,array($object,'method'));
     * // 2. Display the upload table with id "1"; 
     * //it is important to first set and then display the uploadify
     * {template}->uploadify(1);
     * 
     * @param string $id
     * @param string $destination
     * @param array $settings - You can set:<ul>
     * <li>_append_ - array(object,method);use a custom method that returns an array to append to the uploaded files array(which is processed by the $useTable method)</li>
     * <li>_replace_ - array(object,method);use a custom method that returns an array(which is processed by the $useTable method)</li>
     * <li>sizeLimit - the maximum allowed file size</li>
     * <li>fileExt - the allowed file types</li>
     * </ul>
     * @param boolean|array(object,string) $useTable <br/>
     * A. $useTable is true by default; setting it to false prevents the display of a table;<br/>
     * B. The object may be any available instance of a class; similar to call_user_function.<br/>
     * <hr/>
     * <b>How the defined method should look like.</b><br/>
     * It must accept 1 parameter; it may be:<ul>
     * <li>true</li>
     * <li>array(<ul>
     *     <li>'name'=>{string - file name},</li>
     *     <li>'size'=>{int - file size in bytes},</li>
     *     <li>'url'=>{string - file location}</li>
     * </ul>)
     * </li>
     * </ul><br/>
     * Discussion:
     * <ul>
     * <li>If the parameter is true (use === to verify), the method must return an array of table header columns; ex.: array('File','Url');</li>
     * <li>If the parameter is an array(), the method must return an array of the same size with custom information; ex.: array('test.txt','/upload/destination/test.txt');</li>
     * </ul>
     * @return <ul>
     * <li>null - if no $id is passed</li>
     * <li>true - if no $destination is passed; prints the table</li>
     * <li>$this - if the $destination is passed; prepares uploadify</li>
     * </ul>
     */
    public function uploadify($id = null, $destination = null, $settings = array(), $useTable = true) {
        // The ID is necessary
        if (is_null($id)) {
            return null;
        } else {
            $id = trim(htmlentities($id, ENT_QUOTES));
        }

        // Get the input library
        $this->input = octoms('input');

        // Display the HTML
        if (is_null($destination)) {
            // Get the saved values
            $destination = $this->getOne('uploadify_' . $id . '_destination');
            $settings = $this->getOne('uploadify_' . $id . '_settings');
            $useTable = $this->getOne('uploadify_' . $id . '_useTable');

            // List available files?
            if ($useTable === true) {
                // Start the table
                print '<table class="data" id="' . $id . 'table">';

                // Print the table head
                print '<thead><tr><th>File Name</th><th>File Size</th><th>URL</th><th align="center">Actions</th></tr></thead>';

                // Start the table body
                print '<tbody>';

                // Print the files
                if (is_array($files = $this->input->file_list($destination))) {
                    foreach ($files as $file) {
                        print '<tr>' .
                            '<td>' . $file['name'] . '</td>' .
                            '<td>' . round($file['size'] / 1024, 2) . ' KB</td>' .
                            '<td>' . a(urldecode($file['url']), $file['url'], array('target' => '_blank')) . '</td>' .
                            '<td>' . action_btn('delete', $file['name'], 'uploadify_delete') . '</td>' .
                            '</tr>';
                    }
                }

                // Print the table footer
                print '</tbody></table><div class="clear"></div>';
            }
            // Print a custom table
            elseif (is_array($useTable)) {
                if (isset($useTable[1]) && isset($useTable[1]) && method_exists($useTable[0], $useTable[1])) {
                    // Start the table
                    print '<table class="data" id="' . $id . 'table">';

                    // Print the table head
                    print '<thead><tr>' .
                        implode(
                            '',
                            array_map(
                                create_function(
                                    '$str',
                                    'return "<th>".$str."</th>";'
                                ),
                                (array) call_user_func(
                                    array(
                                        $useTable[0],
                                        $useTable[1]
                                    ),
                                    true
                                )
                            )
                        ) .
                        '</tr></thead>';

                    // Start the table body
                    print '<tbody>';

                    // Use the default file list?
                    if (!isset($settings['_replace_'])) {
                        // Get the list of available files for this post
                        $list = $this->input->file_list($destination);

                        // Append some data?
                        if (isset($settings['_append_']) && !is_null($list)) {
                            if (is_array($settings['_append_']) && count($settings['_append_']) == 2 && method_exists($settings['_append_'][0], $settings['_append_'][1])) {
                                $list = array_merge($list, call_user_func($settings['_append_']));
                            }
                        }
                    } else {
                        if (is_array($settings['_replace_']) && count($settings['_replace_']) == 2 && method_exists($settings['_replace_'][0], $settings['_replace_'][1])) {
                            $list = call_user_func($settings['_replace_']);
                        }
                    }

                    // Print the files in a custom way
                    if (is_array($list)) {
                        foreach ($list as $k => $file) {
                            if (is_numeric($k)) {
                                print '<tr>' .
                                    implode(
                                        '',
                                        array_map(
                                            create_function(
                                                '$str',
                                                'return "<td>".$str."</td>";'
                                            ),
                                            (array) call_user_func(
                                                array(
                                                    $useTable[0],
                                                    $useTable[1]
                                                ),
                                                $file
                                            )
                                        )
                                    ) .
                                    '</tr>';
                            }
                        }
                    }

                    // Print the table footer
                    print '</tbody></table><div class="clear"></div>';
                }
            }

            // Show the form
            print '<div id="uploadifyQueue"></div><div class="clear"></div>' .
                    form(null, 'post', array('enctype' => 'multipart/form-data')) .
                    input('file', $id) .
                    form(true);

            // All done
            return true;
        } else {
            // Save the destination
            $this->set('uploadify_' . $id . '_destination', $destination);
            // Save the settings
            $this->set('uploadify_' . $id . '_settings', $settings);
            // Save the useTable
            $this->set('uploadify_' . $id . '_useTable', $useTable);

            // Get the maximum filesize allowed by the system
            $fsSysMax = str_replace(array('K', 'M', 'G'), array(str_repeat('0', 3), str_repeat('0', 6), str_repeat('0', 9)), min(array(ini_get('upload_max_filesize'), ini_get('post_max_size'))));
            // Get the maximum filesize supplied by the user
            $fsUserMax = isset($settings['sizeLimit']) ? $settings['sizeLimit'] : null;
            // Set the size limit
            $sizeLimit = is_null($fsUserMax) ? $fsSysMax : (min(array($fsSysMax, $fsUserMax)));

            // Upload check
            if ($this->input->get('uploadify_check') == 'true') {
                // Get the list of available files for this post
                $list = $this->input->file_list($destination);

                // Turn the output buffer off
                ob_end_clean();

                // Any results?
                if (!is_null($list) && count($list) > 0) {
                    // Get individual file names
                    $filesFound = array();
                    foreach ($list as $el) {
                        $filesFound[] = $el['name'];
                    }

                    // Prepare the response
                    $filesReq = array();
                    foreach ($this->input->sg('post') as $key => $value) {
                        if ($key != 'folder' && in_array($value, $filesFound)) {
                            $filesReq[$key] = $value;
                        }
                    }

                    // Send it as JSON
                    print json_encode($filesReq);
                }
                // Return a blank list
                else {
                    print '[]';
                }
                // Stop here
                exit();
            } elseif ($this->input->file()) {
                foreach ($this->input->file() as $k => $f) {
                    $link = $this->input->uploadWeb . WS . str_replace(DS, WS, $destination) . WS . urlencode($f['name']);
                    try {
                        // Get allowed extensions
                        $allowedExt = isset($settings['fileExt']) ? $settings['fileExt'] : null;

                        // Replace all files
                        $this->input->upload($k, $destination, null, null, array(0, $sizeLimit), $allowedExt, true);
                        $this->api(
                            a(
                                $f['name'],
                                $link,
                                array('target' => '_blank')
                            )
                        );

                        // Get some information about the upload; use the custom method
                        if (isset($useTable[1]) && isset($useTable[1]) && method_exists($useTable[0], $useTable[1])) {
                            $f['url'] = $this->input->uploadWeb . WS . str_replace(DS, WS, $destination) . WS . urlencode($f['name']);
                            $info = call_user_func(array($useTable[0], $useTable[1]), $f);
                        }
                        // Use the default layout
                        else {
                            $info = array(
                                $f['name'],
                                round(filesize($this->input->uploadDir . DS . $destination . DS . $f['name']) / 1024, 2) . ' KB',
                                a(urldecode($link), $link, array('target' => '_blank')),
                                delete_btn($f['name'], 'uploadify_delete')
                            );
                        }

                        // Add it to the response
                        if (isset($info))
                            $this->api_cnt($info);
                    } catch (Exception $e) {
                        // File already exists
                        if ($e->getCode() == 6) {
                            $this->api($e->getMessage() . ' ' .
                                a(
                                    $f['name'],
                                    $link,
                                    array('target' => '_blank')
                                ),
                                false
                            );
                        } else {
                            $this->api($e->getMessage() . var_export($this->input->post(), true), false);
                        }
                    }
                }
            } elseif ($this->input->post('uploadify_delete')) {
                // Try to delete the file
                if (file_exists($f = $this->input->uploadDir . DS . $destination . DS . $this->input->post('uploadify_delete')) && unlink($f)) {
                    $this->api('File "' . $this->input->post('uploadify_delete') . '" was successfully removed.')->api(array('delRow'));
                } else {
                    $this->api('Invalid file specified. Please refresh and try again.', false);
                }

                // Remove this key
                $this->input->del('post', 'uploadify_delete');
            }

            // Load the uploadify module
            $this->jQuery('uploadify', 'content', null,
                array(
                    // The upload size limit is dictated by the server
                    'sizeLimit' => $sizeLimit,
                    // Allowed extensions
                    'fileExt' => isset($settings['fileExt']) ? implode(';', array_map(create_function('$v', 'return "*.".$v;'), $settings['fileExt'])) : '*.*',
                )
            );

            // Use tables?
            if ($useTable != false)
                $this->jQuery('tables', 'content');

            return $this;
        }
    }

    /**
     * URL helper method; return a link to the template or the addon folders
     * 
     * @example 
     * // CSS directory in the template folder
     * {template}->url('css');
     * // CSS file in the CSS directory in the template folder
     * {template}->url('css/file.css');
     * // A CSS folder in the current addon and area
     * {template}->url('css',false);
     * // A CSS folder in the 'content' addon and the current area
     * {template}->url('css',false,'content');
     * // A CSS folder in the 'content' addon and the 'open' area
     * {template}->url('css',false,'content','open');
     * // A custom url - used by the JS and CSS methods
     * {template}->url('http://example.com',null,null,null,true);
     * 
     * @param string $u
     * @param string $tpl
     * @param string $addon
     * @param string $area
     * @param string $custom
     * @return string - URL
     */
    public function url($u = '', $tpl = true, $addon = null, $area = null, $custom = null) {
        // Use a custom location; or http://example.com; or //example.com
        if (!is_null($custom) || strpos($u, '//') !== false) {
            return $u;
        }
        // Default template
        elseif ($tpl === true || is_null($tpl) || is_string($tpl)) {
            return WEBROOT . WS . trim(str_replace(array(ROOT, DS), array('', WS), APP_TPL), WS) . WS . (is_string($tpl) ? $tpl : $this->conf('template')) . WS . (!is_null($area) ? $area : $this->conf('area')) . WS . strval($u);
        }
        // Addon template
        else {
            return WEBROOT . WS . trim(str_replace(array(ROOT, DS), array('', WS), FERV_ADDONS), WS) . WS . (!is_null($addon) ? $addon : $this->conf('addon')) . WS . 'views' . WS . (!is_null($area) ? $area : $this->conf('area')) . WS . strval($u);
        }
    }

    /**
     * Print the user information
     * 
     * @example 
     * // Print the user information
     * {template}->user();
     */
    public function user() {
        // Load the authentification library
        $auth = octoms('auth', OMS_L);

        // If the user is logged in
        if ($auth->is_logged()) {
            // Get the user information
            $user = $auth->user();

            // Print the data
            print a(
                (!empty($user['name']) ? $user['name'] : $user['username']) . ' (' . ucfirst($auth->role_name()) . ')',
                i18n_baseurl('admin/settings/profile')
            );
        }
    }

    /**
     * Display a widget
     * 
     * @example 
     * // Load the method 'method1' from the widget 'w1'
     * {template}->widget('w1'[,'method1'][,array('arg1','arg2')]);
     * // Append the JS and CSS for widget 'w1'
     * // Use before calling $this->js() or $this->css() in your template
     * // Widget controller must have the $JS and $CSS properties defined
     * {template}->widget('w1',true);
     * {template}->widget('w1',false);
     * @throws {template}->bug();
     * 
     * @param string $widget
     * @param string $method
     * @param array $args
     * @return mixed - the result of the requested method
     */
    public function widget($widget, $method = null, $args = array()) {
        // The class name
        $controller = 'w_' . ($widget = strval($widget));

        // Fetch the object
        if (!isset(octoms::$oms_objects[$controller])) {
            // Load the Controller
            if (file_exists($f = FERV_WIDGETS . DS . $widget . DS . 'controllers' . DS . $controller . EXT)) {
                require_once $f;
                // Load the controller
                if (!class_exists($controller)) {
                    return $this->bug('Widget "' . $widget . '" expects controller class "' . $controller . '{}".', 1);
                } else {
                    octoms::$oms_objects[$controller] = new $controller;
                }

                // Is there a model?
                if (file_exists($f = FERV_WIDGETS . DS . $widget . DS . 'models' . DS . $controller . EXT)) {
                    require_once $f;
                    $model = $controller . '_m';

                    if (!class_exists($model)) {
                        $this->bug('Widget "' . $widget . '" expects model class "' . $model . '{}".', 2);
                    } else {
                        octoms::$oms_objects[$controller]->model = new $model;
                    }
                }

                // Bind all the objects to this widget
                foreach (array_keys(octoms::$oms_objects) as $key) {
                    if (strpos($key, 'w_') === false) {
                        $obj = substr($key, 0, strpos($key, '_'));
                        // Bind objects to the controller
                        octoms::$oms_objects[$controller]->$obj = octoms::$oms_objects[$key];

                        // Bind objects to the model (if set)
                        if (isset(octoms::$oms_objects[$controller]->model)) {
                            octoms::$oms_objects[$controller]->model->$obj = octoms::$oms_objects[$key];
                        }
                    }
                }
            } else {
                return $this->bug('Widget "' . $widget . '" expects controller file "' . $controller . EXT . '".', 3);
            }
        }

        // Prepare the JS and CSS? Call this before $this->js() and $this->css() in your template
        if (is_bool($method)) {
            // Add the JS
            if (isset(octoms::$oms_objects[$controller]->JS)) {
                foreach ((array) octoms::$oms_objects[$controller]->JS as $js) {
                    $this->js(
                        array(
                            WEBROOT . WS . trim(str_replace(array(ROOT, DS), array('', WS), FERV_WIDGETS), WS) . WS . $widget . WS . 'views' . WS . 'js' . WS . $js . (false === strpos($js, '.js') ? '.js' : ''),
                            'custom' => true
                        )
                    );
                }
            }

            // Add the CSS
            if (isset(octoms::$oms_objects[$controller]->CSS)) {
                foreach ((array) octoms::$oms_objects[$controller]->CSS as $css) {
                    $this->css(
                        array(
                            WEBROOT . WS . trim(str_replace(array(ROOT, DS), array('', WS), FERV_WIDGETS), WS) . WS . $widget . WS . 'views' . WS . 'css' . WS . $css . (false === strpos($css, '.css') ? '.css' : ''),
                            'custom' => true
                        )
                    );
                }
            }

            // All done
            return $this;
        } else {
            // Set the method
            $method = empty($method) ? 'index' : strval($method);

            // Is it defined?
            if (!method_exists(octoms::$oms_objects[$controller], $method)) {
                return $this->bug('Method "' . $method . '()" not found in controller class "' . $controller . '{}" of widget "' . $widget . '"', 4);
            }

            // Call the method
            return call_user_func_array(array(octoms::$oms_objects[$controller], $method), (array) $args);
        }
    }
}

/* End Of File <template.inc> */