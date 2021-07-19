                <?php if(!defined('OCTOMS')){header('HTTP/1.1 403'); die('{"error":"forbidden"}');}
/**
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */

/*
 * PHP 5.3.x error constants
 */ 
if (!defined('E_STRICT')) define('E_STRICT', 2048);
if (!defined('E_RECOVERABLE_ERROR')) define('E_RECOVERABLE_ERROR', 4096);
if (!defined('E_DEPRECATED')) define('E_DEPRECATED', 8192);
if (!defined('E_USER_DEPRECATED')) define('E_USER_DEPRECATED', 16384);
    
class error_cl {
    /**
     * Class constructor
     * 
     * @return error
     */
    public function __construct() {
        // Use the custom error logger
        ini_set('log_errors', 0);

        // Report all
        error_reporting(E_ALL);

        // Development
        if (!ENVIRONMENT) {
            ini_set('display_startup_errors', 1);
            ini_set('display_errors', 1);
        }
        // Production
        else {
            ini_set('display_startup_errors', 0);
            ini_set('display_errors', 0);
        }

        // Error wizard
        if (ERROR_WIZARD) {
            // Set the error handler and the exception handler
            set_error_handler(array($this, 'error_h'), E_ALL);
            set_exception_handler(array($this, 'exception_h'));
        }
    }

    /**
     * Error table helper
     * 
     * @param string $errfile
     * @param string $errline
     * @param array $debugBacktrace
     * @return string Error table
     */
    public function _error_table($errfile, $errline, $debugBacktrace = array()) {
        if (!function_exists('highlight_line')) {
            octoms('html', OMS_CH);
        }

        // Show the backtrace
        $code = '';
        $debug_bt = array_reverse($debugBacktrace);
        foreach ($debug_bt as $debug) {
            if (isset($debug['line']) && isset($debug['file']) && strpos($debug['file'], 'octoms' . EXT) === false && strpos($debug['file'], 'index.php') === false) {
                if (($debug['file'] == $errfile && $debug['line'] != $errline) || $debug['file'] != $errfile) {
                    $code .= '<h3 class="th">&#x25BC; ' . $debug['file'] . '</h3>';
                    $code .= highlight_line(
                            $debug['file'],
                            $debug['line'],
                            5,
                            array(
                                'leftClass' => 'l',
                                'rightClass' => 'r',
                                'currentClass' => 'cl',
                                'class' => 'code'
                            )
                    );
                }
            }
        }

        // Get the file as an array of lines
        $code .= '<h3 class="th">&#x25BC; ' . $errfile . '</h3>';
        $code .= highlight_line(
                $errfile,
                $errline,
                5,
                array(
                    'leftClass' => 'l',
                    'rightClass' => 'r',
                    'currentClass' => 'cl',
                    'class' => 'code'
                )
        );

        // Done
        return '<div><b>File:</b> ' . $errfile . '<br/><b>Line:</b> ' . $errline . '</div><br/>' . $code;
    }

    /**
     * Export helper
     * 
     * @param mixed $data
     * @return string - export highlight
     */
    public function _export_highlight($data = null) {
        // Prepare the POST and SESSION for export
        if (is_null($data)) {
            $data = array(
                'POST' => isset($_POST) ? $_POST : null,
                'SESSION' => isset($_SESSION) ? $_SESSION : null,
                'FLAGS' => flag(),
                'SQL' => isset(octoms::$oms_info['sql']) ? octoms::$oms_info['sql'] : null
            );
        }

        // Return the highlighted table
        octoms('html', OMS_CH);
        return highlight(
                str_replace(array('<br />', "\n"), "\r\n", var_export(
                    $data,
                    true
                )),
                null,
                array('class' => 'code')
        );
    }

    /**
     * Error handler
     * 
     * @access private
     * @example This should only be used as an error handler
     * 
     * @param int $errno
     * @param string $errstr
     * @param string $errfile
     * @param string $errline
     */
    public function error_h($errno = 0, $errstr = '', $errfile = '', $errline = '') {
        // Used the @ operator ?
        if (error_reporting() == 0 || $errno == E_DEPRECATED)
            return true;

        // Prepare the error title
        switch ($errno) {
            case E_WARNING:$t = "Warning";
                break;
            case E_NOTICE:$t = "Notice";
                break;
            case E_CORE_WARNING:$t = "Core Warning";
                break;
            case E_COMPILE_WARNING:$t = "Compile Warning";
                break;
            case E_USER_ERROR:$t = "User Error";
                break;
            case E_USER_WARNING:$t = "User Warning";
                break;
            case E_USER_NOTICE:$t = "User Notice";
                break;
            case E_STRICT:$t = "Strict Notice";
                break;
            case E_RECOVERABLE_ERROR:$t = "Recoverable Error";
                break;
            case E_DEPRECATED:$t = "Deprecated Notice";
                break;
            case E_USER_DEPRECATED:$t = "User deprecated Notice";
                break;
            default:$t = "Unknown error";
                break;
        }

        // Log this Error
        $c = !flag() ? $this->log($t . ' #' . $errno . ' \'' . $errstr . '\'', $errfile, $errline) : null;

        // Don't load the wizard in production mode
        if (ENVIRONMENT)
            return true;

        // Clean the output buffer
        while (@ob_end_clean());

        // Prepare the info class
        $info = new stdClass();

        // Export
        $info->c = !is_null($c) ? json_encode($c . $this->_export_highlight()) : json_encode($this->_export_highlight());

        // Exception
        $info->x = null;

        // Debug
        $info->d = null;

        // Search string
        $info->s = 'about';

        // Set the error report
        $info->e = json_encode(
                '<div class="error_title error"></div><div class="carved"><b>' . $t . ': <pre>' . $errstr . '</pre></b></div><br/>' .
                $this->_error_table($errfile, $errline, debug_backtrace())
        );

        // AJAX inteface?
        if (isset($_POST['#*#'])) {
            // JSON header
            @header('Content-type:text/json', true);

            // Print the JSON version
            print json_encode(array('octoms_error' => $info));
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
     * Exception handler
     * 
     * @access private
     * @example This should only be used as an exception handler
     * 
     * @param Exception $e
     * 
     */
    public function exception_h($e) {
        // Get the details
        $errno = $e->getCode();
        $errstr = $e->getMessage();
        $errfile = $e->getFile();
        $errline = $e->getLine();

        // Log this Exception
        $c = !flag() ? $this->log('Exception #' . $errno . ' \'' . $errstr . '\'', $errfile, $errline) : null;

        // Don't load the wizard in production mode
        if (ENVIRONMENT)
            return true;

        // Clean the output buffer
        while (@ob_end_clean());

        // Prepare the info class
        $info = new stdClass();

        // Export
        $info->c = !is_null($c) ? json_encode($c . $this->_export_highlight()) : json_encode($this->_export_highlight());

        // Error
        $info->e = null;

        // Debug
        $info->d = null;

        // Search string
        $info->s = 'about';

        // Set the exception report
        $info->x = json_encode(
                '<div class="error_title exception"></div><div class="carved"><b>Exception #' . $errno . ': <pre>' . $errstr . '</pre></b></div><br/>' .
                $this->_error_table($errfile, $errline, $e->getTrace())
        );

        // AJAX inteface?
        if (isset($_POST['#*#'])) {
            // JSON header
            @header('Content-type:text/json', true);

            // Print the JSON version
            print json_encode(array('octoms_error' => $info));
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
     * Log an error to the default location
     * 
     * @example 
     * # Log an error at this file and line
     * $this->error->log('An error occured');
     * # Log an error for another file
     * $this->error->log('An error occured','path/to/file','line number');
     * 
     * 
     * @param string $message
     * @param string $file
     * @param string $line
     * @return Error
     * 
     */
    public function log($message = '', $file = null, $line = null) {
        // The message was logged directly from the model or controller
        if (empty($file) || empty($line)) {
            // Get the debug backtrace
            $info = debug_backtrace();
            $file = $info[0]['file'];
            $line = $info[0]['line'];
            unset($info);
        }

        // Save this information
        try {
            octoms('csv', OMS_CL)->insert(
                array(
                    array(
                        date('H:i:s'),
                        "$file:$line",
                        var_export($message, true),
                        $_SERVER['REMOTE_ADDR'] . $_SERVER['REQUEST_URI'],
                        isset($_SERVER['HTTP_REFERER']) ? $_SERVER['HTTP_REFERER'] : null,
                        var_export($_GET, true),
                        var_export($_POST, true),
                    )
                ),
                $f = APP_ERR . DS . 'log' . DS . date('d-m-Y') . '.csv'
            );
        } catch (Exception $e) {
            return '<div class="error_title warning"></div><div class="carved">This error was not logged because the registry <br/><b>' . $f . '</b><br/> is not writable.</div>';
        }

        return null;
    }

    /**
     * Display an error page
     * 
     * @example 
     * # Set a default 404 page not found
     * $this->error->page();
     * # Set a custom message for the selected error page 
     * $this->error->page(403,'Forbidden message'); 
     * 
     * 
     * @param int $no
     * @param string $message
     */
    public function page($no = 404, $message = '') {
        // Verify the error number
        if (empty($no))
            throw new Exception('You must set a valid HTTP/1.1 error response code');

        // Hide the output buffer
        while (@ob_end_clean());

        // Set the header
        $info = new stdClass();
        $info->errors = array(
            400 => 'Bad Request',
            401 => 'Unauthorized',
            402 => 'Payment Required',
            403 => 'Forbidden',
            404 => 'Not Found',
            405 => 'Method Not Allowed',
            406 => 'Not Acceptable',
            407 => 'Proxy Authentication Required',
            408 => 'Request Timeout',
            409 => 'Conflict',
            410 => 'Gone',
            411 => 'Length Required',
            412 => 'Precondition Failed',
            413 => 'Request Entity Too Large',
            414 => 'Request-URI Too Long',
            415 => 'Unsupported Media Type',
            416 => 'Requested Range Not Satisfiable',
            417 => 'Expectation Failed',
            500 => 'Internal Server Error',
            501 => 'Not Implemented',
            502 => 'Bad Gateway',
            503 => 'Service Unavailable',
            504 => 'Gateway Timeout',
            505 => 'HTTP Version Not Supported',
            509 => 'Bandwidth Limit Exceeded'
        );

        // Set the message
        $info->message = $message;

        // Keep it within the defined error responses
        $info->no = isset($info->errors[$no]) ? $no : 404;

        // Set the template to load
        $tpl = intval($info->no / 100) . 'xx.tpl';

        // No more errors from here on
        error_reporting(0);

        // Set the header
        if (debugging() && !ENVIRONMENT) {
            // Regular response
            header('HTTP/1.0 200 OK');
            header('OctoMS-Error:' . $info->no . ' ' . $info->errors[$info->no]);
        } else {
            // Error page
            header('HTTP/1.0 ' . $info->no . ' ' . $info->errors[$info->no]);
        }

        // Load the error template
        require_once (file_exists(APP_ERR . DS . $tpl)) ? (APP_ERR . DS . $tpl) : (CORE_ERR . DS . $tpl);

        // Nothing beyond this point
        exit();
    }
}

/* End Of File <error.inc> */