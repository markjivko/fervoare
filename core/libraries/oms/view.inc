<?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @subpackage    WP.OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
class view_cl {

    /**
     * Load a view file.
     * 
     * Callable from <ul>
     * <li>any controller: $this->view->load(...);</li>
     * <li>any view file: $this->load(...);</li>
     * </ul>
     * 
     * @example
     * # load the local index.tpl view with no variables
     * $this->view->load('index.tpl');
     * # the local index.tpl uses $variableName
     * $this->view->load('index.tpl',array('variableName'=>'value'));
     * # load the view from the "otherStage"
     * $this->view->load('index.tpl',null,'otherStage');
     * # you can use your own directory structure for the "views" folder
     * $this->view->load('main/index.tpl');
     * # use a custom location
     * $this->view->load('viewName',null,null,'customStage/location');
     *  
     * @param string $viewName
     * @param array $variables
     * @param string $stageName
     * @param string $inApp
     * @return string
     */
    public function load($viewName = null, $variables = array(), $stageName = '', $inApp = '', $info = null) {
        // The view file name must be provided
        if (!empty($viewName)) {
            // Format the filename
            $viewName = str_replace(
                array('\\', '/'),
                DS,
                preg_replace(
                    '/^(.*)\.tpl$/i',
                    '$1',
                    $viewName
                )
            );
        } else {
            return false;
        }

        // Some information on the caller
        $info = (null == $info) ? debug_backtrace() : $info;

        if (isset($info[0])) {
            // Get the information
            $f = $info[0]['file'];

            // Clear some memory
            unset($info);
        } else {
            return false;
        }

        if (!empty($inApp)) {
            $file = APP_ROOT . DS . $inApp . DS . $viewName . '.tpl';
        } else {
            // The stage name is returned as $r[2]
            if (preg_match('%^(.*?)\\' . DS . '([^\\' . DS . ']+)\\' . DS . '(controllers|models|views).*?$%', $f, $r)) {
                $file = $r[1] . DS . (empty($stageName) ? $r[2] : $stageName) . DS . 'views' . DS . $viewName . '.tpl';
            } else {
                // Invalid filename
                return false;
            }
        }

        // Stop the infinite loop of including yourself
        if ($file == $f)
            return false;

        // Get the helper
        $helper = new view_cl_helper($file, $variables);

        // Load the file then destroy the data
        return $helper->l();
    }

    /**
     * Display a view file.
     * 
     * Callable from <ul>
     * <li>any controller: $this->view->display(...);</li>
     * <li>any view file: $this->display(...);</li>
     * </ul>
     * 
     * @example
     * # display the local index.tpl view with no variables
     * $this->view->display('index.tpl');
     * # the local index.tpl uses $variableName
     * $this->view->display('index.tpl',array('variableName'=>'value'));
     * # display the view from the "otherStage"
     * $this->view->display('index.tpl',null,'otherStage');
     * # you can use your own directory structure for the "views" folder
     * $this->view->display('main/index.tpl');
     * 
     * @param string $viewName
     * @param array $variables
     * @param string $stageName
     * @param string $inApp
     * @return view_cl
     */
    public function display($viewName = null, $variables = array(), $stageName = '', $inApp = '') {
        // Print the loaded view file
        print $this->load($viewName, $variables, $stageName, $inApp, debug_backtrace());

        // Return this
        return $this;
    }

    /**
     * Call any method of any controller on any stage.
     * This is useful for addons or resources employed by multiple stages.
     * Callable from <ul>
     * <li>any controller: $this->view->addon(...);</li>
     * <li>any view file: $this->addon(...);</li>
     * </ul>
     * 
     * @example
     * # Load the output of the 'm' method in the 'c' controller on stage 's'
     * $this->addon('s','c','m',array('Parameter'));
     * 
     * @param string $stage
     * @param string $controller
     * @param string $method
     * @param array $parameters
     * @return view_cl
     */
    public function addon($stage = null, $controller = null, $method = null, $parameters = array()) {
        // The parameter must be an array
        if (!is_array($parameters))
            $parameters = array($parameters);

        // We need all of this
        if (empty($stage) || empty($controller) || empty($method))
            return $this;

        // Figure out the file name
        $file = STAGES . DS . $stage . DS . 'controllers' . DS . $controller . EXT;

        // Try to load the file
        if (file_exists($file)) {

            // Load the file
            require_once $file;

            if (class_exists($controller)) {
                ob_start();

                if (method_exists($object = new $controller, $method)) {
                    // Bind all the autoload objects
                    foreach (octoms::$oms_objects as $key => $obj) {
                        $key = substr($key, 0, strrpos($key, '_'));
                        $object->$key = $obj;
                    }

                    // Save the arguments
                    $object->__args = $parameters;

                    // Call the controller
                    call_user_func_array(array($object, $method), $parameters);
                }

                return @ob_get_clean();
            }
        }

        // Return the data
        return false;
    }
}

/**
 * View helper
 * 
 * 
 * @package OctoMS
 * @subpackage OctoMS
 * @version 1.0.0
 */
class view_cl_helper extends view_cl {

    public $data = array();
    public $file = null;
    public $loaded = false;

    public function __construct($file = null, $data = array()) {
        // Save the file
        $this->file = $file;

        // Save the data
        $this->data = (array) $data;
    }

    /**
     * Load the file
     * 
     * @return string - File contents
     */
    public function l() {
        if (!$this->loaded) {
            $this->loaded = true;

            if (file_exists($this->file)) {
                // Start the buffer
                ob_start();

                // Use local variables?
                if (is_array($this->data) && count($this->data) > 0) {
                    foreach ($this->data as $key => $value) {
                        if (!is_numeric($key) && !is_numeric(substr($key, 0, 1)) && $key != 'this') {
                            $$key = $value;
                        }
                    }
                }

                // Load the file
                require $this->file;

                // Clean the variables
                unset($this->data);
                unset($this->file);

                // Clean the string of the UTF-8 start character
                return preg_replace('/\x{FEFF}/iu', '', @ob_get_clean());
            }
        }

        // Clean the variables
        unset($this->data);
        unset($this->file);

        // Nothing to show
        return null;
    }

    /**
     * Test for the existence of a variable and return it or its child elements (if it's an array/object)
     * 
     * @example 
     * // Print the $admin variable
     * print $this->v('admin');
     * // Print $admin['some']['data']
     * print $this->v('admin','some.data');
     * 
     * @param string $variableName
     * @param string $variableChildren
     * @return mixed|null Variable contents or null if not set
     */
    public function v($variableName = null, $variableChildren = null) {
        if (!is_null($this->data) && isset($this->data[$variableName])) {
            $v = $this->data[$variableName];

            if (!is_null($variableChildren)) {
                foreach (explode('.', $variableChildren) as $c) {
                    if (is_object($v))
                        $v = object_to_array($v);
                    if (!is_array($v)) {
                        $v = null;
                        break;
                    }
                    if (isset($v[$c])) {
                        $v = $v[$c];
                    } else {
                        $v = null;
                        break;
                    }
                }
            }
            return $v;
        } else {
            return null;
        }
    }
}
    
/* End Of File <view.inc> */