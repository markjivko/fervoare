<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/**
 * @package   OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
    /**
 * The wizard model
 * 
 * 
 * @package OctoMS
 * @subpackage wizard model
 * 
 */
class wizard_m extends octoms {

    /**
     * The Octoms API url
     */
    const API_URL = 'https://stephino.com/server/api/';

    /**
     * The data to print in a JSON format
     * [{"content":string,"script":array/null,"status":"success"/"error"},...]
     * 
     * @var array
     */
    public $data;

    /**
     * Append data 
     * 
     * @example $this->wizard->append('Result',array('script1','script2'));
     * 
     * @param string $data
     * @param array $scripts
     * @param boolean $success
     * @return wizard_m
     * 
     */
    public function append($data = '', $scripts = array(), $success = true) {
        $this->data[] = array(
            'content' => $data,
            'script' => $scripts,
            'status' => $success ? 'success' : 'error'
        );

        return $this;
    }

    /**
     * Render the available data and stop the script
     * 
     * @example $this->wizard->render();
     * 
     */
    public function render() {
        // Garbage collector
        ob_end_clean();

        // Plaintext
        header('content-type:application/json');

        // <3 JSON
        print json_encode($this->data);

        // End execution
        exit();
    }

    /**
     * Debug a page via cURL
     * 
     * @example $this->wizard->debug('/hello/world');
     * 
     * @param string $page
     * @return string|boolean
     */
    public function debug($page = null) {
        // The page must not be empty
        if (empty($page)) {
            return false;
        }

        // Prepare the cURL request
        $request = $this->_('request', OMS_CL);
        try {
            // Load the local page
            $request->load(
                DOMAIN_PROTOCOL . $_SERVER['HTTP_HOST'] . $page,
                array('__' . md5(BF_SALT) . '__' => true),
                'post',
                array(
                    'cookie' => isset($_COOKIE[COOKIE_NAME]) ? (COOKIE_NAME . '=' . urlencode($_COOKIE[COOKIE_NAME])) : '',
                    'user-agent' => $_SERVER['HTTP_USER_AGENT']
                )
            );

            // Get the request body
            $r = $request->body;

            // Verify the response
            if (base64_decode($r, true)) {
                // Get the debugging information
                $r = json_decode(blowfish_decrypt($r));
                $r->errorPage = isset($request->headers['octoms-error']) ? $request->headers['octoms-error'] : null;
                $r->redirPage = isset($request->headers['octoms-redirect']) ? $request->headers['octoms-redirect'] : null;

                // Load the source file
                if (file_exists($f = STAGES . DS . $r->info->stage . DS . 'controllers' . DS . $r->info->controller . EXT)) {
                    // Get the contents
                    $c = file_get_contents($f);

                    // Find the method employed
                    preg_match('%^[^\e]*?(function\s+' . $r->info->method . '\s*\([^\e]*?\)[^\e]*?\{[^\e]*?\}[^\e]*?)function[^\e]*?$%', $c, $reg);
                    if (isset($reg[1])) {
                        $r->code = $reg[1];
                    } else {
                        $r->code = 'Error: please end your method with "}// end function".';
                    }
                } else {
                    $r->code = 'Error: File "' . $f . '" not found on this server.';
                }

                // Return the result
                return $r;
            } else {
                // Something went wrong
                return false;
            }
        } catch (Exception $e) {
            return $e->getMessage();
        }
    }

    /**
     * Search within a file
     * 
     * @param string $file
     * @param string $term
     * @return array results
     */
    public function searchf($file, $term = '') {
        // Initiate the result
        $f = array();

        // Load the file, if it exists
        if (!file_exists($file)) {
            return $f;
        }
        
        // Get the comment areas
        preg_match_all('%/\*\*(.*?\*/)\s*?(?:public|static|private|protected)*\s*(function[\s]+.*?)+?\(%ims', file_get_contents($file), $reg);

        // If there are comment, there must be information
        if (isset($reg[1])) {
            // Save the comment area
            $r = $reg[1];

            // And the function name
            $r2 = $reg[2];

            // Cleanup some memory
            unset($reg);

            // Search through each comment
            foreach ($r as $k => $v) {
                // Skip this if it is not a method of if it is the constructor
                if (strpos($r2[$k], 'function') === false || strpos($r2[$k], '__construct') !== false) {
                    continue;
                }

                // Split those into key-value pairs (@link http://abc... becomes ['link']='http://abc...')
                preg_match_all('%@(.*?)\s+?([^\e@*]*\*[\s!-?[-~A-Z]*)%', $v, $m);

                // Make keys unique
                $i = 1;
                foreach ($m[1] as $_k => $_v) {
                    if ($_v == 'param') {
                        $m[1][$_k] = $_v . ' ' . $i++;
                    }
                }

                // Whatever remains is the item's description
                $m[1][count($m[1])] = 'description';
                $m[2][count($m[2])] = trim(preg_replace('%@(.*?)\s+?([^\e@*]*\*[\s!-?[-~A-Z]*)%', '', $v));

                // Keys are referenced by $1 and values by $2
                $r[$k] = array_combine($m[1], preg_replace('%/\*|\*/|\*|\t{2,}|\s+.*$%', '', $m[2]));

                // Clear-up some memory
                unset($m);

                // And resource information
                $r[$k]['res']['obj'] = str_replace(array(EXT, DS), '', substr($file, strrpos($file, DS)));

                // Valuable result? (case insensitive search)
                foreach (explode(' ', trim($term)) as $t) {
                    // Not found within the example or no example set 
                    if ((isset($r[$k]['example']) && strpos(strtolower($r[$k]['example']), strtolower($t)) === false) || !isset($r[$k]['example'])) {
                        // Also not found within the description?
                        if (strpos(strtolower($r[$k]['description']), strtolower($t)) === false && strtolower($r[$k]['res']['obj']) != strtolower($t)) {
                            // This is not our match
                            continue 2;
                        }
                    }
                }

                // Add the file location
                $r[$k]['file'] = $file;
                $r[$k]['res']['type'] = strpos($file, 'libraries' . DS) === false ? (strpos($file, 'helpers' . DS) === false ? 'model' : 'helper') : 'library';
                $r[$k]['res']['method'] = $r[$k]['res']['obj'] . ' ' . $r[$k]['res']['type'] . ': ' . trim(str_replace('function', '', $r2[$k])) . '()';
                $r[$k]['res']['loc'] = strpos($file, 'core' . DS) === false ? 'app' : 'core';


                // Highlight the syntax for the example
                if (isset($r[$k]['example'])) {
                    $this->highlight($r[$k]['example']);
                }

                // Add some standard deployment information
                if ($r[$k]['res']['obj'] != 'octoms') {
                    $r[$k]['deploy'] = '###' . "\n" .
                            '# Load the "' . $r[$k]['res']['obj'] . '" ' . $r[$k]['res']['loc'] . ' ' . $r[$k]['res']['type'] . ' if it wasn\'t already loaded - check using the debugger' . "\n" .
                            '$this->_(\'' . $r[$k]['res']['obj'] . '\', OMS_' . ($r[$k]['res']['type'] == 'model' ? 'M' : (($r[$k]['res']['loc'] == 'core' ? 'C' : '') . strtoupper(substr($r[$k]['res']['type'], 0, 1)))) . ');' . "\n" .
                            '# or add this line to your stage\'s' . ($r[$k]['res']['type'] == 'model' ? '' : ' or app\'s') . ' autoload.inc' . "\n" .
                            ($r[$k]['res']['type'] == 'helper' ? '$helpers[] = ' : '$objects[] = ') . '"' . $r[$k]['res']['obj'] . '' . ($r[$k]['res']['loc'] == 'core' ? '' : '|' . substr($r[$k]['res']['type'], 0, 1)) . '";' . "\n" .
                            '###' . "\n";

                    // Highlight the syntax for the deployment info
                    $this->highlight($r[$k]['deploy']);
                }

                // Got to this point - found a match
                $f[$k] = $r[$k];
            }
            unset($r);
        }

        // Return the findings
        return $f;
    }

    /**
     * Search
     * 
     * @example 
     * 
     * @param string $term
     * @param array $location
     * @return 
     * 
     */
    public function search($term, $location = null) {
        // Findings
        $result = array();
        $dirs = array();
        
        // Format the location
        if (!is_array($location)) {
            $location = array($location);
        }

        // Decide where to look
        if (count($location) > 0) {
            foreach ($location as $l) {
                // Search through the core libraries and helpers
                if ($l == 'core') {
                    $dirs[] = CORE_LIB . DS . 'oms';
                    $dirs[] = CORE_LIB;
                    $dirs[] = CORE_HELP;
                    break;
                }

                // Search through the app libraries and helpers
                if ($l == 'app') {
                    $dirs[] = APP_LIB;
                    $dirs[] = APP_HELP;
                    break;
                }

                // Search through some models
                if (strpos($l, 'stage') !== false) {
                    $dirs[] = STAGES . DS . trim(substr($l, strpos($l, '=') + 1)) . DS . 'models';
                    $dirs[] = STAGES . DS . trim(substr($l, strpos($l, '=') + 1)) . DS . 'controllers';
                    break;
                }
                
                $dirs[] = $dirs[] = CORE_LIB . DS . 'oms';
                $dirs[] = CORE_LIB;
            }
        }
        // Or search everywhere
        else {
            $dirs[] = CORE_LIB . DS . 'oms';
            $dirs[] = CORE_LIB;
            $dirs[] = CORE_HELP;
            $dirs[] = APP_LIB;
            $dirs[] = APP_HELP;
            foreach (glob(STAGES . '/*/*', GLOB_ONLYDIR) as $dirPath) {
                if (in_array(basename($dirPath), array('models', 'controllers'))) {
                    $dirs[] = $dirPath;
                }
            }
        }
        
        // Make sure the directories are unique
        $dirs = array_unique($dirs);
        
        // Make a file list
        foreach ($dirs as $dir) {
            if (is_dir($dir) && false !== ($h = opendir($dir))) {
                while (false !== ($f = readdir($h))) {
                    // Append the search results
                    if (is_file($dir . DS . $f) && substr($f, strrpos($f, '.')) == EXT) {
                        $r = $this->searchf($dir . DS . $f, $term);
                        if (count($r) > 0) {
                            $result = array_merge($r, $result);
                        }
                    }
                }
                closedir($h);
            }
        }

        // Return the results, if any
        return $result;
    }

    /**
     * Highlight syntax
     * 
     * @param string &$code
     */
    public function highlight(&$code) {
        $array = array();
        foreach (explode("\n", $code) as $_l => $_c) {
            $array[$_l] = str_replace(
                    '&lt;?php',
                    '',
                    highlight_string('<?php ' . preg_replace('%\s{2,}%', '', $_c), true)
            );
        }
        $code = $array;
    }

    /**
     * API
     */
    public function api($data, $api = '') {
        // Prepare the data
        $data = array(
            'pid' => API_PID,
            'tsk' => blowfish_encrypt(
                json_encode(
                    array(
                        'ip' => $_SERVER['REMOTE_ADDR'],
                        'ua' => $_SERVER['HTTP_USER_AGENT'] . ' - ' . md5(API_TKN),
                        'ttk' => time(),
                        'uid' => API_UID,
                        'tkn' => API_TKN,
                        'cnt' => $data
                    )
                ),
                API_KEY,
                substr(API_KEY, 0, 8)
            )
        );

        // Use the request core library
        $request = $this->_('request', OMS_CL);

        try {
            $request->load(
                wizard_m::API_URL . $api,
                $data,
                'post',
                array(
                    'user-agent' => $_SERVER['HTTP_USER_AGENT'] . ' - ' . md5(API_TKN)
                )
            );

            return $request->body;
        } catch (Exception $e) {
            return $e->getMessage();
        }
    }

    /**
     * Help information
     */
    public function help($term = null, $loc = 'search') {
        if ($loc == 'search') {
            return 'You can search for any term inside your source code; try "about"';
        } else {
            return 'This will show debug information on the specified absolute path';
        }
    }
}
    
/* End Of File <wizard.inc> */