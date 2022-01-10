<?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

class octoms {
    /*
     * The url segments (as they appear in the web browser's address bar)
     * 
     * @var array
     */
    static $oms_url_segments = array();

    /*
     * The progressive transformations the parsed url goest through
     * 
     * @var array
     */
    static $oms_url_morphs = array();

    /*
     * The parsed url segments (after applying the global $route directives)
     * 
     * @var array
     */
    static $oms_parsed_url_segments = array();

    /*
     * An array of loaded model and library objects
     * 
     * @var array
     */
    static $oms_objects = array();

    /*
     * An array of loaded helpers
     * 
     * @var array
     */
    static $oms_helpers = array();

    /*
     * Information about this call (stage name, controller, method etc.)
     * 
     * @var array
     */
    static $oms_info = array();

    /**
     * Dynamic object caller - load a library, model or helper<br/>
     * You can use the <b>{app name}/config/autoload.inc</b> file to load:
     * <ul>
     * <li>Core libraries: add "$objects[] = 'library_name';"</li>
     * <li>Application libraries: add "$objects[] = 'library_name|l';"</li>
     * <li>Core helpers: add "$helpers[] = 'helper_name';"</li>
     * <li>Application helpers: add "$helpers[] = 'helper_name|h';"</li>
     * </ul>
     * But you cannot autoload models from specific stages here.<br/>
     * You can use the <b>{app name}/stages/{stage name}/config/autoload.inc</b> file to load:
     * <ul>
     * <li>Core libraries: add "$objects[] = 'library_name';"</li>
     * <li>Application libraries: add "$objects[] = 'library_name|l';"</li>
     * <li>Stage models: add "$objects[] = 'model_name|m';"</li>
     * <li>Core helpers: add "$helpers[] = 'helper_name';"</li>
     * <li>Application helpers: add "$helpers[] = 'helper_name|h';"</li>
     * <li><i>Unload all app-level libraries:</i> add "$objects[] = '(-app)';"</li>
     * <li><i>Unload all app-level helpers:</i> add "$helpers[] = '(-app)';"</li>
     * </ul>
     * But you cannot autoload models from other stages here.
     * Use "$this->_('model_name',OMS_M,null,'other_stage');" inside your controller/model for this purpose.
     * 
     * @example 
     * # Your controller/model must extend octoms in order to use this method
     * # class {class_name} extends octoms {...
     * # load the 'test' core library
     * $this->_('test');
     * # load the 'test' helper
     * $this->_('test', OMS_H); 
     * # load the 'test' model as 't'
     * $this->_('test', OMS_M, 't');
     * # load the 'test' model as 't2' from the 's' stage
     * $this->_('test', OMS_M, 't2', 's');
     * # etc. (see the $type parameter)
     * 
     * @param string $object - the object/helper to call
     * @param int $type - the object type<ul>
     *     <li>OMS_M - model</li>
     *     <li>OMS_H - helper</li>
     *     <li>OMS_L - library</li>
     *     <li>OMS_CL - core library (default)</li>
     *     <li>OMS_CH - core helper</li>
     * </ul>
     * @param string $name - desired name
     * @param string $stage - optional stage to load the model from
     * @param boolean $forceStage - load the optional stage model even if this is called from a valid controller
     * @return object|boolean
     * 
     */
    public final function _($object = null, $type = OMS_CL, $name = null, $stage = '', $forceStage = false) {
        // Format the object's name
        if (empty($object)) {
            throw new Exception('Please provide an object or helper to employ.');
            return false;
        } else {
            $object = strtolower($object);
        }

        // Info on the caller
        $i = debug_backtrace();

        // Called from the Core class?
        $cfc = (strpos($i[0]['file'], 'octoms' . EXT) !== false);

        // Figure out the object's path
        switch ($type) {
            case OMS_L: $f = APP_LIB . DS . $object . EXT;
                break;
            case OMS_H: $f = APP_HELP . DS . $object . EXT;
                break;
            case OMS_M: {
                    if ((strpos($i[0]['file'], 'controllers') === false && strpos($i[0]['file'], 'models') === false) || $forceStage)
                        $f = STAGES . DS . $stage . DS . 'models' . DS . $object . EXT;
                    else
                        $f = preg_replace('/^(.*?)(controllers|models).*?$/', '$1models' . DS . $object . EXT, $i[0]['file']);break;
                }
            case OMS_CL: $f = CORE_LIB . DS . 'oms' . DS . $object . EXT;
                break;
            case OMS_CH: $f = CORE_HELP . DS . $object . EXT;
                break;
        }

        // Check if the object was already set
        switch ($type) {
            case OMS_L: {
                    $o = preg_replace('/^(.*?\\\\)*(.*?)$/', '$2_l', $object);
                    $set = isset(self::$oms_objects[$o]);
                    break;
                }
            case OMS_H: {
                    $o = $object . '_h';
                    $set = isset(self::$oms_helpers[$o]);
                    break;
                }
            case OMS_M: {
                    $o = preg_replace('/^(.*?\\\\)*(.*?)$/', '$2_m', $object);
                    $set = isset(self::$oms_objects[$o]);
                    break;
                }
            case OMS_CL: {
                    $o = preg_replace('/^(.*?\\\\)*(.*?)$/', '$2_cl', $object);
                    $set = isset(self::$oms_objects[$o]);
                    break;
                }
            case OMS_CH: {
                    $o = $object . '_ch';
                    $set = isset(self::$oms_objects[$o]);
                    break;
                }
        }

        // Employ objects
        if (in_array($type, array(OMS_L, OMS_CL, OMS_M))) {
            if (!$set) {
                if (file_exists($f)) {
                    require_once $f;
                    self::$oms_objects[$o] = new $o;
                } else {
                    throw new Exception('Invalid file (' . $f . ') and object (' . $o . ') to load.');
                    return false;
                }
            }

            // Prepare this object's name
            $name = empty($name) ? substr($o, 0, strrpos($o, '_')) : $name;

            // Set a local variable?
            if (!$cfc) {
                if (!isset($this->$name)) {
                    // Bind previously loaded objects to the model
                    $this->$name = self::$oms_objects[$o];
                    if ($type == OMS_M) {
                        foreach (self::$oms_objects as $key => $obj) {
                            if ($o != $key) {
                                $key = substr($key, 0, strrpos($key, '_'));
                                $this->$name->$key = $obj;
                            }
                        }
                    }

                    return $this->$name;
                } else {
                    return $this->$name;
                }
            } else {
                return self::$oms_objects[$o];
            }
        }
        // Employ helpers
        elseif (in_array($type, array(OMS_H, OMS_CH))) {
            if (!$set) {
                if (file_exists($f)) {
                    require_once $f;
                    self::$oms_helpers[$o] = true;
                }
            }
        }
        // Invalid call
        else {
            throw new Exception('Invalid type of resource to employ.');
            return false;
        }
    }

    /**
     * Cascade style initialization
     * 
     * @access limited
     */
    public function __construct() {
        // Get the info on the caller
        $info = debug_backtrace();

        // Don't allow child classes to reconstruct Octoms
        if ($info[0]['file'] == ROOT . DS . 'index.php') {
            // Force debugging?
            if (!ENVIRONMENT && isset($_GET[':' . DEBUG_KEYWORD])) {
                $this->__invoke();
            }

            // No magic quotes or pathinfo fix
            ini_set('magic_quotes_runtime', 0);
            ini_set('cgi.fix_pathinfo', 0);
            ini_set('default_charset', 'UTF-8');
            if (isset($GLOBALS['db_character_set']))
                unset($GLOBALS['db_character_set']);

            // @Windows:
            set_include_path(ROOT);

            // @php 5.3:
            if (ini_get('date.timezone') == '')
                date_default_timezone_set('GMT');

            // Script exectution start time
            self::$oms_info['start'] = microtime(true);

            // Preserve self
            self::$oms_info['self'] = $this;

            // Ignore the _GET params from the request uri
            $_SERVER['REQUEST_URI'] = strpos($_SERVER['REQUEST_URI'], '?') !== false ? substr($_SERVER['REQUEST_URI'], 0, strpos($_SERVER['REQUEST_URI'], '?')) : $_SERVER['REQUEST_URI'];

            // Get the forwardslash-free $_SERVER['PATH_INFO'] equivalent
            $path_info = trim(preg_replace('%^' . WEBROOT . '(.*)$%i', '$1', $_SERVER['REQUEST_URI']), WS);

            // Break the path_info into URL segments; ignore the _GET params
            self::$oms_url_segments = !empty($path_info) ? explode(WS, $path_info) : array();

            // Get the last element
            $last = end(self::$oms_url_segments);
            self::$oms_info['ext'] = false;
            if (null !== $last && false !== strpos($last, '.')) {
                // Get the extension
                if (strlen(self::$oms_info['ext'] = substr($last, strrpos($last, '.') + 1)) == 0) {
                    self::$oms_info['ext'] = false;
                }

                // Replace the last element of the URL
                $path_info = substr($path_info, 0, strrpos($path_info, '.'));
                self::$oms_url_segments[count(self::$oms_url_segments) - 1] = substr($last, 0, strrpos($last, '.'));
            }

            // Find the language; {site}/{language - 2 characters}/{page}; default: en
            self::$oms_info['lang'] = DEFAULT_LANGUAGE;
            if (preg_match('%^([a-z]{2}/?)%i', $path_info, $reg)) {
                if (strpos($reg[1], '/') !== false || strlen($path_info) == strlen($reg[1])) {
                    // Get the language particle
                    $path_info = preg_replace('%^' . $reg[1] . '%i', '', $path_info);
                    self::$oms_info['lang_particle'] = trim(strtolower($reg[1]), '/');

                    // Is this language allowed?
                    if (in_array(self::$oms_info['lang_particle'], explode(',', LANGUAGES))) {
                        self::$oms_info['lang'] = self::$oms_info['lang_particle'];
                    }

                    // Remove it from the url segments
                    array_shift(self::$oms_url_segments);
                }
            }

            // Clean the _GET superglobal    
            unset($_GET[WS . $path_info]);

            // Parse the $path_info according to the global $route directives
            if (!isset(self::$oms_url_segments[0]) || self::$oms_url_segments[0] != '~help')
                $this->__oms5($path_info);

            // Break this up into parsed URL segments
            self::$oms_parsed_url_segments = !empty($path_info) ? explode(WS, $path_info) : array();

            // Dispatch the request
            $this->__oms1(self::$oms_parsed_url_segments);
        } else {
            return false;
        }
    }

    /**
     * Display the help console
     * 
     * @example $this(); # PHP >5.3.x; display the help console
     * $this('help term'); # PHP >5.3.x; search for "help term" inside the help console
     * $this->__invoke(); # PHP <5.3.x; same as $this();
     * 
     * @param string $s - the help term
     * @param boolean $export - wether or not to export $s
     */
    public final function __invoke($s = null, $export = false) {
        // Help is available in development mode and if we haven't called the debugger
        if (ENVIRONMENT || debugging()) {
            return $this;
        }

        // Clean the output buffer
        while (@ob_end_clean());

        $info = new stdClass();

        // Export the variable?
        $info->c = ($export !== false) 
            ? json_encode(
                highlight(
                    str_replace(
                        array('<br />', "\n"), 
                        "\r\n", 
                        var_export(
                            $s,
                            true
                        )
                    ),
                    null,
                    array('class' => 'code')
                )
            ) 
            : null;

        // The help term must be a string
        $info->s = (empty($s) || $export !== false) ? 'about' : strval($s);

        // Get the Debug information
        $info->d = $_SERVER['REQUEST_URI'];

        // Set the error report
        $info->e = null;
        $info->x = null;

        // AJAX inteface?
        if (isset($_POST['#*#'])) {
            // JSON header
            @header('Content-type:text/json', true);

            // Print the JSON version
            print json_encode(array('octoms_help' => $info));
        } else {
            // This is always html
            @header('Content-type:text/html', true);

            // Load the file
            require_once CORE . DS . 'wizard' . DS . 'views' . DS . 'index.tpl';
        }

        // That was it!
        exit();
    }

    /**
     * @ignore No cloning
     */
    public function __clone() {
        return isset(octoms::$oms_info['self']) ? octoms::$oms_info['self'] : null;
    }

    /**
     * Log calls to undefined octoms methods
     * 
     * @param string $a - method name
     * @param array $b - parameters
     * @return void
     */
    public function __call($a, $b) {
        return isset(octoms::$oms_objects['error_cl']) 
            ? trigger_error('Call to undefined OCTOMS method "' . $a . '()"') 
            : null;
    }

    /**
     * @ignore Display encrypted debugging information
     * 
     * @access private
     */
    public function __oms0() {
        // Defined functions
        $f = get_defined_functions();

        // Defined constants
        $c = get_defined_constants(true);

        // Get the headers
        $headers = implode("\n", headers_list()) . "\n";
        foreach ($_SERVER as $h => $v)
            if (preg_match('%HTTP_(.+)%', $h, $hp))
                $headers .= ucwords(strtolower($hp[1])) . ": $v\n";

        // Information to save
        $save = array(
            'exec' => (microtime(true) - self::$oms_info['start']),
            'memory_max' => memory_get_peak_usage(),
            'memory' => memory_get_usage(),
            'buffer' => ob_get_contents(),
            'headers' => $headers,
            'info' => array(
                'stage' => self::$oms_info['stage'],
                'controller' => self::$oms_info['controller'],
                'method' => self::$oms_info['method'],
                'parameters' => self::$oms_info['parameters']
            ),
            'objects' => array_keys(self::$oms_objects),
            'helpers' => array_keys(self::$oms_helpers),
            'functions' => $f['user'],
            'session' => isset($_SESSION) ? $_SESSION : null,
            'get' => isset($_GET) ? $_GET : null,
            'post' => isset($_POST) ? $_POST : null,
            'flags' => flag(),
            'url_morphs' => self::$oms_url_morphs,
            'sql' => isset(self::$oms_info['sql']) ? self::$oms_info['sql'] : null
        );

        // Clean the output buffer
        while (@ob_end_clean());

        // Export the data
        print @blowfish_encrypt(json_encode($save));
    }

    /**
     * @ignore Dispatch the request to the apropiate stage
     * 
     * @access private
     * 
     * @param array $seg - parsed url segments
     */
    private function __oms1($seg) {
        // Start the output buffer
        ob_start();

        // Get the stage, controller and method
        if (!empty($seg)) {
            // Load the stage
            $s = $seg;
            $seg = array_slice($seg, 3);
            $this->__oms2(isset($s[0]) ? $s[0] : '', isset($s[1]) ? $s[1] : '', isset($s[2]) ? $s[2] : '', $seg);
        } else {
            // Load the default
            $this->__oms2();
        }

        // Nothing beyond this point
        exit();
    }

    /**
     * @ignore Load given stage - include the file, instantiate the inner class and call the
     * given method with the provided parameters
     * 
     * @access private
     * 
     * @param string $name
     * @param string $controller
     * @param string $method
     * @param array $parameters
     * @return boolean
     */
    private function __oms2($name = DEF_STAGE, $controller = DEF_CONTR, $method = DEF_METHO, $parameters = array()) {
        // Debugging?
        if (debugging() && !ENVIRONMENT) {
            register_shutdown_function(array($this, '__oms0'));
        }

        // Load the core libraries
        $this->__oms3();

        // The stage name is a must; prevent access to secret methods
        if (!empty($name) && substr($method, 0, 1) != '_') {
            // Disambiguation
            if (empty($controller))
                $controller = $name;
            if (empty($method))
                $method = 'index';

            // The wizard is part of the core application
            $f = (($name == '~help' && !ENVIRONMENT) ? CORE . DS . ($name = 'wizard') : STAGES . DS . strtolower($name)) . DS . 'controllers' . DS . $controller . EXT;

            // Load the controller file
            if (@file_exists($f)) {
                // Load the file
                require_once $f;

                // Instantiate the class
                if (class_exists($controller)) {
                    // If the method exists, load it...
                    $object = new $controller;
                    if (method_exists($object, $method) || method_exists($object, '__call')) {
                        // Bind automatically loaded models, libraries and helpers to this object
                        $this->__oms4($object, $name);

                        // Parse the parameters
                        foreach ($parameters as $k => $v) {
                            $parameters[$k] = urldecode($v);
                        }

                        // Save this information
                        self::$oms_info['stage'] = $name;
                        self::$oms_info['controller'] = $controller;
                        self::$oms_info['method'] = $method;
                        self::$oms_info['parameters'] = $parameters;

                        // Perform the method call
                        return strpos($method, '_') !== 0 ? call_user_func_array(array($object, $method), $parameters) : self::$oms_objects['error_cl']->page(404);
                    }
                }
            }
        }

        // Something went wrong; display a 404 error
        self::$oms_objects['error_cl']->page(404);
    }

    /**
     * @ignore Automatically load the core libraries (as defined in the core/config/autoload.inc)
     * 
     * @access private
     */
    private function __oms3() {
        if (file_exists(CORE_CONFIG . DS . 'autoload' . EXT)) {
            $objects = array();
            $helpers = array();
            require_once CORE_CONFIG . DS . 'autoload' . EXT;

            // Then the libraries
            if (count($helpers) > 0) {
                foreach ($helpers as $h) {
                    $this->_($h, OMS_CH);
                }
            }

            // Then the libraries
            if (count($objects) > 0) {
                foreach ($objects as $obj) {
                    $this->_($obj);
                }
            }
        }
    }

    /**
     * @ignore Bind automatically loaded models, libraries and helpers to the provided model
     * 
     * @access private
     * 
     * @param &object $object
     * @param string $stage
     */
    private function __oms4(&$object, $stage) {
        // Where should we look for the autoload values?
        $locations = array(
            's' => STAGES . DS . $stage . DS . 'config' . DS . 'autoload' . EXT,
            'a' => APP_CONFIG . DS . 'autoload' . EXT
        );

        // Get all the defined autoload arrays - for objects (o) and helpers (h)
        $o = array();
        $h = array();
        foreach ($locations as $k => $l) {
            if (file_exists($l)) {
                $objects = array();
                $helpers = array();
                require_once $l;

                if (count($objects) > 0) {
                    // Load the objects
                    foreach ($objects as $o) {
                        if (strpos($o, '|') !== false) {
                            $exp = explode('|', $o);
                            $o = $exp[0];
                            switch ($exp[1]) {
                                case 'l': $t = OMS_L;
                                    break;
                                case 'm': $t = OMS_M;
                                    break;
                                default: $t = OMS_CL;
                                    break;
                            }
                        } else {
                            $t = OMS_CL;
                        }
                        if (strpos($o, '(') === false)
                            $this->_($o, $t, null, ($t == OMS_M ? $stage : null));
                    }

                    // Skip the app objects? - optimized implementation (but counterintuitive)
                    if ($k == 's' && in_array('(-app)', $objects))
                        break;
                }
                if (count($helpers) > 0) {
                    // Load helpers thus far
                    foreach ($helpers as $h) {
                        if (strpos($h, '|') !== false) {
                            $exp = explode('|', $h);
                            $h = $exp[0];
                            switch ($exp[1]) {
                                case 'h': $t = OMS_H;
                                    break;
                                default: $t = OMS_CH;
                                    break;
                            }
                        } else {
                            $t = OMS_CH;
                        }
                        if (strpos($h, '(') === false)
                            $this->_($h, $t);
                    }

                    // Skip the app helpers?
                    if ($k == 's' && in_array('(-app)', $helpers))
                        break;
                }
            }
        }

        // Bind all objects to the one provided
        if (!empty(self::$oms_objects)) {
            foreach (self::$oms_objects as $key => $obj) {
                $key = substr($key, 0, strrpos($key, '_'));
                $object->$key = $obj;
            }
        }
    }

    /**
     * Parse the route according to the global and stage $route directives
     * Available wildcards:
     * <ul>
     * <li>':blank' - Search for a blank character</li>
     * <li>':any' - At least one character</li>
     * <li>':num' - A number</li>
     * <li>':s' - The current stage</li>
     * <li>':c=' - Gets replaced with 'stage/', load a controller</li>
     * </ul>
     * 
     * @access private
     * @param &string $path_info
     */
    private function __oms5(&$path_info = null) {
        // Where to look for the routes?
        $locations = array(
            'a' => APP_ROOT . DS . 'config' . DS . 'routes' . EXT,
            's' => APP_ROOT . DS . 'stages' . DS . '%s' . DS . 'config' . DS . 'routes' . EXT,
        );

        // Start parsing the routing info
        foreach ($locations as $k => $c) {
            // The stage is decided last
            if ($k == 's') {
                $stage = substr($path_info, 0, strpos($path_info, WS));
                $c = sprintf($c, $stage);
            }

            // Load the file if it exists
            if (@file_exists($c)) {
                // Remove current rules
                $route = array();

                // Load the configurations
                require_once $c;

                // Parse the route
                if (count($route) > 0) {
                    $path_info = $this->__oms6($path_info, $route, ($k == 's' ? $stage : ''));
                }
            }
        }
    }

    /**
     * @ignore The parse_route helper; parse the string with regexp rules
     * @access private
     * @param string $string
     * @param array $rules
     * @param string $stage
     * @return string parsed url segments
     */
    private function __oms6($string = '', $rules = array(), $stage = '') {
        // If there is a $rules array, go!
        if (isset($rules) && !empty($rules) && is_array($rules)) {
            // No RegExp
            if (isset($rules[$string])) {
                $r = array($string, $rules[$string]);
                $string = $rules[$string];
                self::$oms_url_morphs[] = array('r' => $r, 'u' => $string);
            } else {
                // Perform a RegExp replacement
                foreach ($rules as $k => $v) {
                    // Wildcards to RegExp
                    $k = str_replace(array(':blank', ':any', ':num', ':s', ':c='), array('', '.+', '[0-9]+', $stage, $stage . WS), $k);

                    // Perform replacements
                    if (preg_match('%^' . $k . '$%', $string)) {
                        // Using backreferences
                        if (strpos($v, '$') !== false && strpos($k, '(') !== false) {
                            $string = preg_replace('%^' . $k . '$%', $v, $string);
                        } else {
                            $string = $v;
                        }

                        self::$oms_url_morphs[] = array('r' => array($k, $v), 'u' => $string);

                        // Found our match!
                        break;
                    }
                }
            }
        }

        // That is it!
        return $string;
    }
}

/**
 * Return the OctoMS instance or an OctoMS object
 * 
 * @example 
 * // Get an OctoMS instance; just the default methods and properties
 * octoms();
 * // Get the 'morph' OctoMS object
 * octoms('morph');
 * // Get the 'template' OctoMS library
 * octoms('template',OMS_L);
 * // Get the 'input' OctoMS core library
 * octoms('template',OMS_CL);
 * 
 * @param string $object - object name
 * @return object|null on error
 * 
 */
function octoms($object = null, $objectType = OMS_CL) {
    if (!is_null($object)) {
        $result = null;
        foreach (array_keys(octoms::$oms_objects) as $key) {
            $root = substr($key, 0, strrpos($key, '_'));
            if ($key == $object || $root == $object) {
                $result = octoms::$oms_objects[$key];
                break;
            }
        }
        if (is_null($result)) {
            return octoms::$oms_info['self']->_($object, $objectType);
        } else {
            return $result;
        }
    } else {
        return isset(octoms::$oms_info['self']) ? octoms::$oms_info['self'] : null;
    }
}

/**
 * Help
 * 
 * @example 
 * // Call the help wizard
 * help();
 * // Search for the "foo" term
 * help('foo');
 * 
 * @param string $s - The search term
 * @param boolean $export - Use the wizard to export a variable
 * @return The Wizard
 * 
 */
function help($s = null, $export = false) {
    return octoms()->__invoke($s, $export);
}

/**
 * Debugging session?
 * 
 * @example 
 * 
 * @return boolean - True if the page gathers debugging information
 * 
 */
function debugging() {
    // Only perform this action once
    if (isset($_POST['__' . md5(BF_SALT) . '__'])) {
        // Set the debugging mode
        octoms::$oms_info['debugging'] = true;

        // Unset this secret key
        unset($_POST['__' . md5(BF_SALT) . '__']);
    }

    // Are we debugging?
    return isset(octoms::$oms_info['debugging']);
}

/**
 * Get the framework's language based on the format of the current page
 * 
 * @example 
 * // Get the current language ("en")
 * lang();
 * // Get the language particle (that appears in the url)
 * // If the language particle was not defined in the LANGUAGE constant
 * // the lang() will default to english ("en")
 * lang(true);
 * // Set to french
 * lang('fr');
 * // Set to a language that is not defined in the LANGUAGE constant
 * // Returns false
 * lang('foobar');
 * 
 * 
 * @param string $set
 * @return string|boolean - The current language/ True if the set language is defined
 * in the LANGUAGES constant, False otherwise
 * 
 */
function lang($set = null) {
    if (is_null($set)) {
        return octoms::$oms_info['lang'];
    } elseif (true === $set) {
        return isset(octoms::$oms_info['lang_particle']) ? octoms::$oms_info['lang_particle'] : null;
    } else {
        if (in_array($set, explode(',', LANGUAGES))) {
            octoms::$oms_info['lang'] = $set;
            return true;
        } else {
            return false;
        }
    }
}

/**
 * Flag helper
 * Used to store data for the variable export during an error or exception
 * 
 * @example 
 * // Set the flag data
 * flag($someData);
 * // Set multiple flags
 * flag($data1,$data2);
 * // Return all flags
 * flag();
 * 
 * @param mixed [$data1[,$data2][,...]]
 * @return boolean|array|null - True if flag was set; Array, list of set flags or null if none was set 
 */
function flag($data = null) {
    // Stop here
    if (!debugging())
        return;

    // Get the information about the caller
    $info = debug_backtrace();
    $info = $info[0];

    // Set multiple flags
    if (count($args = func_get_args()) > 1) {
        foreach ($args as $a) {
            octoms::$oms_info['flags']
                    [str_replace(DS, '/', $info['file'])]
                    ['line ' . $info['line']]
                    [] = $a;
        }
        return true;
    }
    // Set one flag
    elseif (count($args) == 1) {
        octoms::$oms_info['flags']
                [str_replace(DS, '/', $info['file'])]
                ['line ' . $info['line']]
                [] = $args[0];
        return true;
    }
    // Or return all saved flags
    else {
        return isset(octoms::$oms_info['flags']) ? octoms::$oms_info['flags'] : null;
    }
}
    
/* End Of File <octoms.inc> */