<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   OctoMS
 * @subpackage    WP.OctoMS
 * @copyright (c) 2011 Mark Jivko https://markjivko.com
 * @license   GPL v3+ (https://gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @since         OctoMS 0.0.1
 */
    
class request_cl {

    /**
     * Store ther response code
     * 
     * @var int
     */
    public $code;

    /**
     * Store the response headers
     * 
     * @var array
     */
    public $headers;

    /**
     * Store the response body
     * 
     * @var string
     */
    public $body = '';

    /**
     * Count the redirects
     */
    public $redirects = 0;

    /**
     * Defined html methods
     * 
     * @var array
     */
    public $http_methods = array(
        'GET',
        'POST'
    );

    /**
     * Set the maximum number of redirects allowed
     * 
     * @var int
     */
    public $maxRedirects = 5;

    /**
     * Load a given web page with the set driver
     */
    public function load($url = null, $data = null, $method = 'post', $headers = null, $driver = null) {
        // Clean the driver
        if (!in_array($driver, array('curl', 'fopen'))) {
            $driver = 'curl';
        }

        // Clean the method
        if (!in_array($method, $this->http_methods)) {
            $method = 'post';
        }

        // Load the driver
        return call_user_func(array($this, 'load_' . $driver), $url, $data, $method, $headers);
    }

    /**
     * Load using CURL
     */
    public function load_curl($url = null, $data = null, $method = 'post', $headers = null) {
        // Create a new cURL resource
        $ch = curl_init();

        // Set the data
        if ($method == 'post') {
            // Set URL
            curl_setopt($ch, CURLOPT_URL, str_replace(' ', '%20', $url));

            // Set the data
            curl_setopt($ch, CURLOPT_POST, true);
            curl_setopt($ch, CURLOPT_POSTFIELDS, http_build_query((array) $data));
        } elseif ($method == 'get') {
            // Set URL
            curl_setopt($ch, CURLOPT_URL, str_replace(' ', '%20', $url . '?' . http_build_query((array) $data)));
        } else {
            // Set URL
            curl_setopt($ch, CURLOPT_URL, str_replace(' ', '%20', $url));
        }

        // NO SSL verification
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);

        // Set the headers
        if (!is_null($headers) && is_array($headers)) {
            $httpheaders = array();
            foreach ($headers as $key => $value) {
                $httpheaders[] = sprintf(
                        '%s: %s',
                        $key,
                        $value
                );
            }
            curl_setopt($ch, CURLOPT_HTTPHEADER, $httpheaders);
        }

        // Always return the header
        curl_setopt($ch, CURLOPT_HEADER, true);

        // Follow the location
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_MAXREDIRS, $this->maxRedirects);

        // Return the content
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);

        // Don't allow infinite waiting time
        curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 4);
        curl_setopt($ch, CURLOPT_TIMEOUT, 8);

        // Grab URL and pass it to the browser
        $r = curl_exec($ch);

        // Get the transfer information
        $info = curl_getinfo($ch);

        // Get the response code
        $this->code = $info['http_code'];

        // Get the redirects count
        $this->redirects = $info['redirect_count'];

        // Get the headers
        $h = preg_split('%(?:\r?\n){2}%i', substr($r, 0, $info['header_size']));

        // Get the last non-empty header
        $header = '';
        while (empty($header)) {
            $header = array_pop($h);
        }

        // And save it
        $this->_getHeaders($header);

        // Get the body
        $this->body = substr($r, $info['header_size']);

        // close cURL resource, and free up system resources
        curl_close($ch);
    }

    /**
     * Load a web page
     * 
     * @param string $url
     * @param mixed $data
     * @param string $method
     * @param array $headers
     */
    public function load_fopen($url = null, $data = null, $method = 'post', $headers = null) {
        // Allow URL Fopen
        if (!ini('allow_url_fopen')) {
            throw new Exception('You must enable "allow_url_fopen" from php.ini.');
        }

        // Throw an exception if the user tries to load a https:// page without the openssl extension
        if (strpos($url, 'https://') === 0 && !in_array('openssl', get_loaded_extensions())) {
            throw new Exception('OpenSSL extension not loaded.');
        }

        // Prepend HTTP://?
        if (strpos($url, '//') === false) {
            $url = 'http://' . DOMAIN . '/' . trim($url, '/');
        }

        // Prepare the headers
        $p = array();
        if (!is_null($data)) {
            if (in_array($method = strtoupper((string) $method), $this->http_methods)) {
                $p['http']['method'] = $method;
                $p['http']['content'] = http_build_query((array) $data);
                $headers['Content-type'] = 'application/x-www-form-urlencoded';
            }
        }

        // Prepare the headers
        if (is_array($headers)) {
            $p['http']['header'] = array();
            foreach ($headers as $key => $value) {
                $p['http']['header'][] = ucfirst(strtolower($key)) . ': ' . $value;
            }
        }

        // Get the file
        if (false !== $fh = @fopen($url, 'rb', false, stream_context_create($p))) {
            // Get the meta data
            $meta_data = stream_get_meta_data($fh);

            // Set the headers
            $this->_getHeaders(implode("\r\n", $meta_data['wrapper_data']));

            // Set the code
            $this->_getCode(isset($meta_data['wrapper_data'][0]) ? $meta_data['wrapper_data'][0] : null);

            // Process redirects
            if ($this->isRedirect() && $this->redirects < $this->maxRedirects) {
                if (isset($this->headers['location'])) {
                    if (is_array($this->headers['location'])) {
                        $url = array_pop($this->headers['location']);
                    } else {
                        $url = $this->headers['location'];
                    }

                    // Recursively load the page
                    $this->redirects++;

                    return $this->load($url, $data, $method, $headers);
                }
            } elseif (!$this->isError()) {
                // Set the body
                while (!feof($fh)) {
                    $this->body .= fread($fh, 8192);
                }
            }

            // Close the file handle
            fclose($fh);
        } else {
            $this->code = null;
            $this->headers = null;
            $this->body = null;
        }

        // Set the last page redirected
        if ($this->redirects > 0) {
            $this->headers['octoms-redirect'] = $url;
        }

        return $this;
    }

    /**
     * Was the response an error?
     * 
     * @return boolean
     * @package OctoMS
     */
    public function isError() {
        return in_array(floor($this->code / 100), array(4, 5));
    }

    /**
     * Was the page a redirect?
     * 
     * @return boolean
     * @package OctoMS
     */
    public function isRedirect() {
        return floor($this->code / 100) == 3 || isset($this->headers['location']);
    }

    /**
     * Get the code from the response string; helper
     * 
     * @param response_string $str
     * @return request_cl
     * @package OctoMS
     */
    protected function _getCode($str = '') {
        // Perform the regex search
        preg_match('%^HTTP/[\d\.x]+ (\d+)%', $str, $m);

        // Set blank if not found
        $this->code = isset($m[1]) ? (int) $m[1] : null;

        return $this;
    }

    /**
     * Get the headers from the response string; helper
     * 
     * @param response_string $str
     * @return request_cl
     * @package OctoMS
     */
    protected function _getHeaders($str = '') {
        // Stop if the string is empty
        if (empty($str)) {
            $this->headers = null;
            return $this;
        }

        // Prepare the response
        $headers = array();

        // Split the headers from the body
        $parts = preg_split('%(?:\r?\n){2}%i', $str);
        if (!$parts[0]) {
            $this->headers = null;
            return $this;
        }

        // Split headers part to lines
        $last_header = null;

        // Parse the headers line by line
        foreach (explode("\n", $parts[0]) as $line) {
            $line = trim($line, "\r\n");
            if ($line == "")
                break;

            if (@preg_match('%^([\w-]+):\s*(.+)%', $line, $m)) {
                $last_header = $key = strtolower($m[1]);
                $value = $m[2];
                if (isset($headers[$key])) {
                    if (!is_array($headers[$key])) {
                        $headers[$key] = array($headers[$key], $value);
                    } else {
                        $headers[$key][] = $value;
                    }
                } else {
                    $headers[$key] = $value;
                }
            } elseif (@preg_match('%^\s+(.+)$%', $line, $m) && !is_null($last_header)) {
                if (is_array($headers[$last_header])) {
                    end($headers[$last_header]);
                    $last_key = key($headers[$last_header]);
                    $headers[$last_header][$last_key] .= $m[1];
                } else {
                    $headers[$last_header] .= $m[1];
                }
            }
        }

        // Set the headers
        $this->headers = (count($headers) > 0) ? $headers : null;

        // Return this
        return $this;
    }
}

/* End Of File <request.inc> */