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

if (!function_exists('a')) {
    /**
     * HTML anchor element
     * 
     * @example 
     * // Link to example.com
     * a('Example','http://example.com');
     * // Empty link (href="#")
     * a('Foo');
     * // Link with class 'bar'
     * a('Foo','http://example.com',array('class'=>'bar'));
     * 
     * 
     * @param string $text
     * @param string $href
     * @param array $options
     * @return String - HTML anchor element, null if $text is not set
     */
    function a($text = null, $href = null, $options = array()) {
        // Anchor body
        if (empty($text)) {
            return null;
        }

        // Anchor href
        if (empty($href)) {
            $href = '#';
        }
        
        // Custom extension
        if (!preg_match('%^https?\:\/\/%i', $href) && strlen($extension = octoms('settings', OMS_L)->get('seo.general_extension'))) {
            if (!preg_match('%\.' . preg_quote($extension) . '$%i', $href)) {
                $href .= '.' . $extension;
            }
        }
        
        // Seed the options
        $options = array_merge((array) $options, array('href' => $href));

        // Create an anchor tag
        return tag('a', $options, $text);
    }
}

if (!function_exists('baseurl')) {
    /**
     * Return a link to the webroot
     * 
     * @example 
     * // Return a link to 'admin'
     * baseurl('admin');
     * 
     * 
     * @param string $link
     * @return string - URL from website webroot
     */
    function baseurl($link = null) {
        return WEBROOT . WS . (!is_null($link) ? $link : '');
    }
}

if (!function_exists('button')) {
    /**
     * HTML button element
     * 
     * @example 
     * // Button ID is automatically set as the button name
     * button('Foo','nameAndId');
     * // No ID or name are set if $name is empty
     * button('Foo');
     * // Use a custom class
     * button('Foo','bar',array('class'=>'baz'));
     * 
     * 
     * @param string $text
     * @param string|array $name
     * @param array $options
     * @return String - HTML button element, null if $text is not set
     */
    function button($text = null, $name = null, $options = array()) {
        // Text is mandatory
        if (empty($text)) {
            return null;
        }

        // Return the input type butotn element
        return input('button', $name, $text, $options);
    }
}

if (!function_exists('cdata')) {
    /**
     * CDATA - surround text with cdata tags
     * 
     * @example 
     * // Simple CDATA encapsulation
     * cdata('Random text.');
     * // CDATA encapsulation for use inside script tags
     * cdata('function(){}',true);
     * 
     * @param string $data
     * @param boolean $js - wether to use this inside script tags
     * @return String - CDATA encapsulated text
     */
    function cdata($data = null, $js = false) {
        return ($js ? '//' : '') . "<![CDATA[\n" . strval($data) . "\n" . ($js ? '//' : '') . "]]>";
    }
}

if (!function_exists('css')) {
    /**
     * HTML Style element or stylesheet Link element
     * 
     * @example
     * // Add a Link element
     * css('path/to.css');
     * // Set the media as 'screen'
     * css('path/to.css','screen');
     * // Add a style element
     * css('body{}',null,false);
     * 
     * @param string $srcOrData
     * @param string $media
     * @param boolean $remote - true for link, false for inline CSS
     * @return String - HTML css Style or Link element
     */
    function css($srcOrData = null, $media = null, $remote = true) {
        // The source or data are needed
        if (empty($srcOrData)) {
            return null;
        }

        // All done
        return ($remote === true) ? lnk($srcOrData, null, null, $media) : tag('style', array('type' => 'text/css'), $srcOrData);
    }
}

if (!function_exists('form')) {
    /**
     * HTML form element
     * 
     * @example 
     * // >Form start; default POST to same page
     * form();
     * // Form start; POST to '/foo'
     * form('/foo');
     * // Form start; GET to '/foo'
     * form('/foo','GET');
     * // Form start; extra class 'bar'
     * form('/foo','GET',array('class'=>'bar'));
     * 
     * // >Form end
     * form(true);
     * 
     * 
     * @param string|boolean $action If set to true, the form end tag will be returned
     * @param string $method
     * @param array $options
     * @return HTML form element
     */
    function form($action = null, $method = null, $options = array()) {
        // Prepare the options
        $method = empty($method) ? 'post' : $method;
        $options = array_merge((array) $options, array('action' => $action, 'method' => $method));

        // Return the start or the end of the form element
        return ($action !== true) ? str_replace('</form>', '', tag('form', $options)) : '</form>';
    }
}

if (!function_exists('here')) {
    /**
     * URL relative to the current location
     * 
     * @example 
     * // Ex. current location is /admin/content; return /admin/content/a/b
     * here('a/b')
     * 
     * @param string $link
     * @return String - URL relative to the current location
     */
    function here($link = null) {
        return i18n_baseurl(implode(WS, octoms::$oms_url_segments)) . (!is_null($link) ? WS . $link : '');
    }
}

if (!function_exists('highlight')) {
    /**
     * Highlight a PHP string as a table
     * 
     * @example 
     * // Highlight "$foo='bar';"
     * highlight("$foo='bar';");
     * // Start the count at line 5
     * highlight("",5);
     * // Use a class for the table
     * highlight("",null,array('class'=>'baz'));
     * 
     * 
     * @param string $str
     * @param int $startCount
     * @param array $options - Special keys<ul>
     * <li>leftClass - the HTML class the left TD should have</li>
     * <li>rightClass - the HTML class the right TD should have</li>
     * <li>currentLine - the line to highlight as current</li>
     * <li>currentClass - the HTML class for both TDs that highlights the selected line</li>
     * </ul>
     * @return string - highlightet string, null on error
     */
    function highlight($str = '', $startCount = 1, $options = null) {
        // Prepare the code
        if (strlen($str) == 0) {
            return null;
        } else {
            if (strpos($str, '<?php') !== 0) {
                $str = "<?php \r\n" . $str;
            }
        }

        // Start line
        if (is_null($startCount) || !is_numeric($startCount)) {
            $startCount = 1;
        }

        // Some important options
        $leftClass = null;
        $rightClass = null;
        $currentClass = null;
        $currentLine = null;
        if (isset($options['leftClass'])) {
            $leftClass = $options['leftClass'];
            unset($options['leftClass']);
        }
        if (isset($options['rightClass'])) {
            $rightClass = $options['rightClass'];
            unset($options['rightClass']);
        }
        if (isset($options['currentClass'])) {
            $currentClass = $options['currentClass'];
            unset($options['currentClass']);
        }
        if (isset($options['currentLine'])) {
            $currentLine = $options['currentLine'];
            unset($options['currentLine']);
        }

        // Prepare the result
        $result = "";

        // Parse each line
        foreach (explode("\r", highlight_string($str, true)) as $line) {
            if (preg_match('%^<br[\s]*/>%i', $line)) {
                // Get the line
                $line = preg_replace('%<br[\s]*/>([^\n]*)\n*[^\e]*%i', '$1', $line);

                // Append this to the result
                $result .= sprintf(
                        '<tr><td%s>%s</td><td%s>%s</td></tr>' . "\n",
                        !is_null($leftClass) ? (' class="' . $leftClass . (($startCount == $currentLine) ? ' ' . $currentClass : '') . '"') : '',
                        $startCount,
                        !is_null($rightClass) ? (' class="' . $rightClass . (($startCount == $currentLine) ? ' ' . $currentClass : '') . '"') : '',
                        $line
                );
                $startCount++;
            }
        }

        // Any result?
        if (strlen($result) > 0) {
            return tag('table', $options, $result);
        } else {
            return null;
        }
    }
}

if (!function_exists('highlight_line')) {
    /**
     * Highlight a line in a file
     * 
     * @example 
     * highlight_line()
     * 
     * @param string $file
     * @param int $line
     * @param int $padding
     * @param array $options() - Special keys<ul>
     * <li>leftClass - the HTML class the left TD should have</li>
     * <li>rightClass - the HTML class the right TD should have</li>
     * <li>currentLine - the line to highlight as current</li>
     * <li>currentClass - the HTML class for both TDs that highlights the selected line</li>
     * </ul>
     */
    function highlight_line($file, $line = 1, $padding = 5, $options = array()) {
        if (file_exists($file)) {
            // Get the file as an array
            $file = file($file);

            // Format the line
            if (is_null($line) || !is_numeric($line)) {
                $line = 1;
            }

            // Set the padding
            if (!is_numeric($padding)) {
                $padding = 5;
            }

            // Calculate the margins
            $top = $line - $padding < 1 ? 1 : $line - $padding;
            $bottom = $line + $padding;

            // Get all the lines
            $result = array();
            for ($i = $top; $i <= $bottom; $i++) {
                if (isset($file[$i - 1])) {
                    $result[$i] = str_replace("\r\n", '', $file[$i - 1]);
                }
            }

            // Get the start line
            $resultKeys = array_keys($result);
            $startCount = array_shift($resultKeys);

            // Append some options
            $options['currentLine'] = $line;

            // Format the code
            return highlight(implode("\r\n", $result), $startCount, $options);
        } else {
            return null;
        }
    }
}

if (!function_exists('i18n_baseurl')) {
    /**
     * Internationalized link to the root
     * 
     * @example 
     * // If the user language is German, this will return '/de/foo.html'
     * i18n_baseurl('foo.html');
     * 
     * @param string $link
     * @return string - URL
     */
    function i18n_baseurl($link = null, $toLanguage = null) {
        // Return an internationalized url to this relative link
        if (is_null($toLanguage)) {
            // Return the default language
            if (octoms::$oms_info['lang'] == DEFAULT_LANGUAGE) {
                return baseurl($link);
            } else {
                return WEBROOT . WS . octoms::$oms_info['lang'] . WS . (!is_null($link) ? $link : '');
            }
        }
        // Return the internationalized url in the specified language
        else {
            if (is_null($link)) {
                $link = '';
            } else {
                // Remove the internationalization, if there is one
                if (isset(octoms::$oms_info['lang_particle']) && 0 === strpos($link, WEBROOT . WS . octoms::$oms_info['lang_particle'])) {
                    $link = trim(
                            preg_replace(
                                    '%^' . WEBROOT . WS . octoms::$oms_info['lang_particle'] . '%i',
                                    '',
                                    $link
                            ),
                            WS
                    );
                } elseif (0 === @strpos($link, WEBROOT) && WEBROOT != '') {
                    $link = trim(
                            preg_replace(
                                    '%^' . WEBROOT . '%i',
                                    '',
                                    $link
                            ),
                            WS
                    );
                } else {
                    $link = trim($link, WS);
                }
            }

            return WEBROOT . WS . ($toLanguage == DEFAULT_LANGUAGE ? '' : $toLanguage . WS) . $link;
        }
    }
}

if (!function_exists('img')) {
    /**
     * HTML img tag
     * 
     * @example 
     * // Image at foo.bar
     * img('foo.bar');
     * // Add class 'baz'
     * img('foo.bar',array('class'=>'baz'));
     * 
     * @param string $src
     * @return array $options, or null if $src is not set
     */
    function img($src = null, $options = array()) {
        if (empty($src)) {
            return null;
        }

        // Add the source property
        $options = array_merge((array) $options, array('src' => $src));

        // Create an image anchor
        return tag('img', $options, null, true);
    }
}

if (!function_exists('input')) {
    /**
     * Input element
     * 
     * @example 
     * // Text input element with name and id 'foo', value 'bar', class 'baz'
     * input('text','foo','bar',array('class'=>'baz'));
     * 
     * // Text input with name 'foo'
     * // If no value is provided, the $_POST['foo'] field will be used as value
     * input('text',array('foo'));
     * 
     * // Text input with name 'foo' and id 'bar'
     * input('text',array('foo','bar'));
     * 
     * @param string $type Input types allowed:<ul>
     * <li>button</li>
     * <li>checkbox</li>
     * <li>file</li>
     * <li>hidden</li>
     * <li>image</li>
     * <li>password</li>
     * <li>radio</li>
     * <li>reset</li>
     * <li>submit</li>
     * <li>text</li>
     * </ul>; Default: text
     * @param string|array $name String for Name and Id; array(name,id);
     * @param string $value
     * @param string $options
     * @return String - HTML input element
     */
    function input($type = 'text', $name = null, $value = null, $options = array()) {
        // Prepare the type
        if (!in_array($type, array('button', 'checkbox', 'file', 'hidden', 'image', 'password', 'radio', 'reset', 'submit', 'text'))) {
            $options['type'] = 'text';
        } else {
            $options['type'] = $type;
        }

        // Add the name?
        if (!empty($name)) {
            if (is_array($name)) {
                $options['name'] = array_shift($name);
            } else {
                $options['name'] = $name;
            }
        }

        // Add the value
        if (empty($value) && isset($options['name']) && is_array($_POST) && isset($_POST[$options['name']])) {
            $value = $_POST[$options['name']];
        }
        $options['value'] = $value;
        if ('hidden' !== $type && isset($options['name'])) {
            $options['id'] = $options['name'];
        }

        // Return the input
        return tag('input', $options, null, true);
    }
}

if (!function_exists('js')) {
    /**
     * HTML script tag
     * 
     * @example 
     * // Load a remote javascript
     * js('path/to.js');
     * 
     * // An inline JavaScript
     * js('function(){}',false);
     * 
     * @param string $srcOrData
     * @param boolean $remote
     * @return String - HTML script tag
     */
    function js($srcOrData = null, $remote = true) {
        // This is mandatory
        if (empty($srcOrData)) {
            return null;
        }

        // All done
        return ($remote === true) ? tag('script', array('type' => 'text/javascript', 'src' => $srcOrData)) : tag('script', array('type' => 'text/javascript'), cdata($srcOrData, true));
    }
}

if (!function_exists('l')) {
    /**
     * HTML list element
     * 
     * @example
     * # $list = array(
     * #    'foo',
     * #    'bar'=>array('a','b'), # child elements
     * #    '[*]baz', # this element gets class="current"
     * #    '[abc]element1', # this element gets id="abc"
     * #    '[rel=example]element2' # this element gets rel="example"
     * #    '[id=a1,class=c1]element3' # this element gets id="a1" and class="c1"
     * #    '[*class=c1]element4' # this element gets class="current c1"
     * #    '[*][foo bar]baz' # only the first brackets "[*]" are parsed and removed
     * #);
     * // Return a HTML list with the above structure
     * l($list);
     * 
     * // Make it ordered
     * l($list,true);
     * 
     * // Give it a "foobar" class
     * l($list,true,array('class'=>'foobar');
     * 
     * // Add some content before the first LI
     * l($list,true,array('innerhtml'=>'content before li'));
     * 
     * @param array $elements Mark current list elements with a starting asterisk (*)
     * @param boolean $ordered - true for ol, false for ul; default false
     * @param array $options
     * @return String - HTML list element
     */
    function l($elements = array(), $ordered = false, $options = array()) {
        $result = '';
        if (is_array($elements)) {
            foreach ($elements as $key => $element) {
                // Add some li options?
                $opt = array();
                if (preg_match('/^\[([^\]]*?)\]/i', is_numeric($key) ? $element : $key, $r)) {
                    // Is this the current item?
                    if (strpos($r[1], '*') === 0) {
                        // Add class current
                        $opt['class'] = 'current';

                        // Remove the asterisk
                        $r[1] = substr($r[1], 1);
                    }

                    // Multiple options
                    if (false !== strpos($r[1], ',')) {
                        $exp = explode(',', $r[1]);
                    } else {
                        $exp = array($r[1]);
                    }

                    // Parse each one; if no = is found, presume that is the ID
                    foreach ($exp as $e) {
                        if (false !== strpos($e, '=')) {
                            $exp2 = explode('=', $e);
                            // Append the class?
                            if (isset($opt[$exp2[0]])) {
                                $opt[$exp2[0]] .= ' ' . $exp2[1];
                            }
                            // A new item
                            else {
                                $opt[$exp2[0]] = $exp2[1];
                            }
                        } elseif (strlen($e) > 0) {
                            $opt['id'] = $e;
                        }
                    }

                    // Remove these instructions
                    if (is_numeric($key)) {
                        $element = preg_replace('/^\[([^\]]*?)\]/i', '', $element);
                    } else {
                        $key = preg_replace('/^\[([^\]]*?)\]/i', '', $key);
                    }
                }

                // Elements with no children
                if (is_numeric($key)) {
                    $result .= tag('li', $opt, $element);
                }
                // Append some children
                else {
                    $result .= tag('li', $opt, $key . l($element, $ordered));
                }
            }

            // Encapsulate the list
            return tag($ordered ? 'ol' : 'ul', $options, $result);
        } else {
            // Return a simple element
            return l(array($elements), $ordered, $options);
        }
    }
}

if (!function_exists('label')) {
    /**
     * Label html element
     * 
     * @example 
     * // Print a label for the "name" input
     * label('The Name','name');
     * 
     * // Also add the class "foo"
     * label('The Name','name',array('class'=>'foo'));
     * 
     * // Make it mandatory (marked with an *)
     * labels('The Name','name',null,true);
     * 
     * // Make it mandatory (marked with 'needed')
     * labels('The Name','name',null,'needed');
     * 
     * @param string $text
     * @param string $for
     * @param array $options
     * @param boolean $required
     * @return string - HTML label element
     */
    function label($text = null, $for = null, $options = array(), $required = false) {
        // Create a label
        return empty($for)
            ? null
            : tag(
                'label',
                array_merge(
                    (array) $options,
                    array('for' => $for)
                ),
                strval($text) . (
                    $required !== false 
                        ? tag(
                            'span',
                            array('class' => 'required'),
                            is_bool($required) ? '*' : $required
                        )
                        : ''
                )
            );
    }
}

if (!function_exists('lang_select')) {
    /**
     * Return a list of available languages
     * 
     * @example 
     * lang_select()
     */
    function lang_select() {
        // Prepare the list
        $langs = array();
        foreach (explode(',', LANGUAGES) as $l) {
            $langs[i18n_baseurl(here(), $l)] = $l;
        }

        // Return the select
        return select(
            $langs,
            i18n_baseurl(here(), lang()),
            null,
            array(
                'onchange' => 'window.location.href=this.value+window.location.hash;'
            )
        );
    }
}

if (!function_exists('lnk')) {
    /**
     * Link element
     * 
     * @example 
     * // A Stylesheet link
     * lnk('path/to.css','stylesheet');
     * 
     * @param string $href
     * @param string $rel Allowed:<ul>
     *    <li>alternate</li>
     *    <li>appendix</li>
     *    <li>bookmark</li>
     *    <li>chapter</li>
     *    <li>contents</li>
     *    <li>copyright</li>
     *    <li>glossary</li>
     *    <li>help</li>
     *    <li>home</li>
     *    <li>index</li>
     *    <li>next</li>
     *    <li>prev</li>
     *    <li>section</li>
     *    <li>start</li>
     *    <li>stylesheet</li>
     *    <li>subsection</li>
     * </ul>
     * @param string $type
     * @param string $media Allowed:<ul>
     * <li>screen</li>
     *    <li>tty</li>
     *    <li>tv</li>
     *    <li>projection</li>
     *    <li>handheld</li>
     *    <li>print</li>
     *    <li>braille</li>
     *    <li>aural</li>
     *    <li>all</li>
     * </ul>
     * @param array $options
     * @return string - HTML link element
     */
    function lnk($href = null, $rel = null, $type = null, $media = null, $options = array()) {
        // Set allwed values
        $relAllowed = array('alternate', 'appendix', 'bookmark', 'chapter', 'contents', 'copyright', 'glossary', 'help', 'home', 'index', 'next', 'prev', 'section', 'start', 'stylesheet', 'subsection');
        $mediaAllowed = array('screen', 'tty', 'tv', 'projection', 'handheld', 'print', 'braille', 'aural', 'all');

        // Clean some of the input
        if (!in_array($rel, $relAllowed)) {
            $rel = 'stylesheet';
        }
        if (!in_array($media, $mediaAllowed)) {
            $media = 'all';
        }
        if (empty($type)) {
            $type = 'text/css';
        }

        // Prepare the options
        $options = array_merge((array) $options, array('rel' => $rel, 'href' => strval($href), 'type' => $type, 'media' => $media));

        // Return the link
        return tag('link', $options, null, true);
    }
}

if (!function_exists('radio')) {
    /**
     * Radio array
     * 
     * @example 
     * radio()
     */
    function radio($alternatives = array(), $selected = null, $options = array(), $showText = true) {
        if (is_array($alternatives) && count($alternatives) > 0) {
            // Set the name
            if (is_string($options) && strpos($options, '=') === false) {
                $options = array('name' => $options);
            }

            // Set the type
            $options['type'] = 'radio';

            $alts = '';
            foreach ($alternatives as $k => $v) {
                if ($k == $selected) {
                    $options['checked'] = 'checked';
                } else {
                    unset($options['checked']);
                }
                $options['title'] = $v;
                $options['value'] = $k;
                $alts .= tag('input', $options, null, true) . ($showText ? tag('span', null, $v) : '');
            }

            return $alts;
        } else {
            return null;
        }
    }
}

if (!function_exists('rule')) {

    /**
     * Set a rule for a form field
     * 
     * @example 
     * // The 'foo' field should be an email
     * rule('foo','email','Provide a valid e-mail address.');
     * 
     * @param string $fieldName - HTML name of form field to impose rule on
     * @param string $rule - The rule to check for; comma-separated<ul>
     * <li>length:5-200    # length between 5 and 200 characters</li>
     * <li>length:200    # length of maximum 200 characters</li>
     * <li>length:200+    # length of minimum 200 characters</li>
     * <li>regex:/foo/i    # wether the regex matches (part of) the field</li>
     * <li>confirm:field_foo # wether this field has the same value as field_foo</li>
     * <li>required    # the field is required (length>0)</li>
     * <li>number    # the field is a number</li>
     * <li>email    # the field is a valid email address</li>
     * <li>url    # the field is a valid URL</li>
     * </ul>
     * @param string $errorMessage - The message the user should see in the event of a failure
     * @return string - empty string on success, $errorMessage on failure
     */
    function rule($fieldName, $rule, $errorMessage) {
        // Start the session
        if (session_id() == '') {
            octoms('session')->start();
        }

        // Append the rule
        $_SESSION['__flash__']['_form_rules_'][$fieldName] = array($rule, $errorMessage);
        if (isset($_SESSION['__flash__']['_form_errors_'][$fieldName])) {
            unset($_SESSION['__flash__']['_form_errors_'][$fieldName]);
            return $errorMessage;
        }

        // Nothing to show
        return '';
    }
}

if (!function_exists('select')) {
    /**
     * Create a select tag
     * 
     * @example 
     * // A simple select
     * select(array('a'=>'Foo','b'=>'Bar'));
     * // Select 'Bar'
     * select(array('a'=>'Foo','b'=>'Bar'),'b');
     * // Select with name and id 'abc'
     * select(array('a'=>'foo','b'=>'bar','b','abc'));
     * // Select with name 'abc'
     * select(array('a'=>'foo','b'=>'bar','b',array('abc')));
     * // Select with name 'abc' and id 'def'
     * select(array('a'=>'foo','b'=>'bar','b',array('abc','def')));
     * // Add the class "baz"
     * select(array('a'=>'Foo','b'=>'bar'),'b',null,array('class'=>'baz'));
     * 
     * @param array $alternatives
     * @param string $selected
     * @param array $options
     * @return string - HTML select element
     */
    function select($alternatives, $selected = null, $name = null, $options = array()) {
        if (is_array($alternatives) && count($alternatives) > 0) {
            // Add the name?
            if (!empty($name)) {
                if (is_array($name)) {
                    $options['name'] = array_shift($name);
                    $options['id'] = array_shift($name);
                } else {
                    $options['name'] = $name;
                    $options['id'] = $name;
                }
            }

            // Get the selection from the POST
            if (empty($selected) && isset($options['name']) && @isset($_POST[$options['name']])) {
                $selected = $_POST[$options['name']];
            }

            // Prepare the alternatives
            $alts = '';
            foreach ($alternatives as $k => $v) {
                $opt = array('value' => $k);
                if (strval($k) == strval($selected)) {
                    $opt['selected'] = 'selected';
                }
                $alts .= tag('option', $opt, $v);
            }

            return tag('select', $options, $alts);
        } else {
            return null;
        }
    }
}

if (!function_exists('submit')) {
    /**
     * The submit html button
     * 
     * @example
     * // &lt;input type="submit" value="Send"/&gt; 
     * submit('Send');
     * // &lt;input type="submit" value="Send" class="x"/&gt;
     * submit('Send',false,array('class'=>'x'));
     * 
     * 
     * @param string $value - The submit text
     * @param boolean $img - True for image, False for text (default)
     * @param array $options
     * @return string - HTML submit input element
     */
    function submit($value = null, $img = false, $options = array()) {
        if (empty($value)) {
            return null;
        }

        // Use an image instead?
        if (is_string($img)) {
            $options = array_merge((array) $options, array('src' => $img, 'alt' => $value));
        }

        // Set a name?
        $name = (isset($options['name'])) ? $options['name'] : null;

        // Return the submit button
        return input(is_string($img) ? 'image' : 'submit', $name, $value, $options);
    }
}

if (!function_exists('t')) {
    /**
     * Translation helper. 
     * Uses '{stage}/i18n/{language}.inc' files.
     * Format: PHP files with these arrays: <ul>
     * <li>$m - for the model files in this stage</li>
     * <li>$v - for the view files in this stage</li>
     * <li>$c - for the controller files in this stage</li>
     * </ul>
     * Example:
     * // This is the translation of "Hello %1!' in romanian (file "i18n/ro.inc")
     * $c[1] = 'Salut %1!';
     * 
     * @example 
     * // Prints "Hello world!"
     * print t(__FILE__,1,'Hello %1!','world');
     * 
     * 
     * @param string $FILE - Use __FILE__ to populate this;
     * @param int $id - The id of the text to translate
     * @param string $text - The text to translate; parameters are accepted; 
     * the parameter format is %{number}
     * @param [,...] string - Parameters for the $text in order
     * @return string - Translated string or the default English text or null if 
     * you didn't call this from a model, controller or view file
     */
    function t($FILE, $id, $text) {
        // Valid call?
        if (preg_match('/(.*?)(models|controllers|views)(.+)/i', $FILE, $regs)) {
            // Save the configurations
            if (!isset(octoms::$oms_info['i18n'][$regs[1]]) && lang() != 'en') {
                $m = array();
                $v = array();
                $c = array();
                // Load the language file
                if (file_exists($f = $regs[1] . DS . 'i18n' . DS . lang() . EXT)) {
                    require_once $f;
                }
                octoms::$oms_info['i18n'][$regs[1]] = array(
                    'm' => $m,
                    'v' => $v,
                    'c' => $c
                );
                unset($m);
                unset($v);
                unset($c);
            }

            // Compute the file type
            $type = substr($regs[2], 0, 1);

            // Get the string
            if (isset(octoms::$oms_info['i18n'][$regs[1]][$type][$id])) {
                $text = octoms::$oms_info['i18n'][$regs[1]][$type][$id];
            }

            // Get the arguments
            global $args;
            $args = array_slice(func_get_args(), 3);

            // Perform the replacement
            $text = preg_replace_callback(
                    '/%([0-9]+)/i',
                    create_function(
                            '$m',
                            'global $args;return isset($args[$m[1]-1])?$args[$m[1]-1]:"";'
                    ),
                    $text
            );

            // Unset the arguments set as global
            unset($args);

            // All done
            return $text;
        } else {
            return null;
        }
    }
}

if (!function_exists('tag')) {
    /**
     * Create a HTML tag
     * 
     * @example 
     * // Section tag
     * tag('section');
     * // Add class 'hello'
     * tag('section',array('class'=>'hello'));
     * tag('section','class="hello"');
     * // Add some content
     * tag('section',null,'foobar');
     * // Return a short tag
     * tag('br',null,null,true);
     * // Prepend a content to a span (right after &lt;span&gt;)
     * tag('span',array('prepend'=>'smth'));
     * // Append a content to a span (right before &lt;/span&gt;)
     * tag('span',array('append'=>'smth'));
     * 
     * 
     * @param string $name
     * @param array|string $options
     * @param string $content
     * @param boolean $short - if set to true, the $content is ignored
     * @return string - HTML element
     */
    function tag($name = null, $options = array(), $content = null, $short = false) {
        // Nothing to create if the name is empty
        if (empty($name)) {
            return null;
        }

        // Prepare the attributes
        $attr = array();
        $options = (array) $options;

        // Append option?
        if (isset($options['append'])) {
            $content = $content . $options['append'];
            unset($options['append']);
        }

        // Prepend options?
        if (isset($options['prepend'])) {
            $content = $options['prepend'] . $content;
            unset($options['prepend']);
        }

        // Parse the rest of the keys
        foreach (array_keys($options) as $key) {
            if (!is_numeric($key)) {
                $attr[] = $key . '="' . htmlentities(strval($options[$key]), ENT_QUOTES, 'UTF-8') . '"';
            } else {
                $attr[] = strval($options[$key]);
            }
        }

        // Return the tag
        return '<' . $name . (strlen($attr = implode(' ', $attr)) > 0 ? ' ' . $attr : '') . ($short ? '/>' : '>' . $content . '</' . $name . '>');
    }
}

if (!function_exists('textarea')) {
    /**
     * Create a textarea html tag
     * 
     * @example 
     * // A simple empty textarea
     * textarea();
     * 
     * // A textarea with 'foo' as content
     * textarea('foo');
     * 
     * // Set the textarea name to 'bar' and ID to 'baz'
     * textarea('foo',array('bar','baz'));
     * 
     * // Name and id are 'bar'; class='baz'
     * textarea('foo','bar',array('class'=>'baz'));
     * 
     * @param string $content
     * @param string $name
     * @param array $options
     * @return string HTML textarea element
     */
    function textarea($content = null, $name = null, $options = array()) {
        // Add the name?
        if (!empty($name)) {
            if (is_array($name)) {
                $options['name'] = array_shift($name);
            } else {
                $options['name'] = $name;
            }
        }

        // Default to what is sent via $_POST
        if (empty($content) && isset($options['name']) && @isset($_POST[$options['name']])) {
            $content = $_POST[$options['name']];
        }

        // Prepare the "value" property
        $options['value'] = htmlentities($content, ENT_QUOTES, 'UTF-8');

        // Create the textarea
        return tag('textarea', $options, $content);
    }
}

if (!function_exists('validate')) {
    /**
     * Validate a form
     * 
     * @example 
     * // IMPORTANT: Call before using the rule() function!
     * if (true === $v = validate()) {
     *     # All went well
     * } else {
     *     # $v = array('fieldName'=>'Error message'[,{field}=>{message}]);
     *     # this may be used for AJAX requests
     * }
     * 
     * @return boolean|array - true if all went well, array of {field}=>array({error message},{error details}) on errors
     */
    function validate() {
        // Check each field recursively
        if (count($args = func_get_args()) > 1) {
            $input = octoms('input');
            $field = $input->post($args[0]);
            $rule = $args[1][0];
            $errorMessage = $args[1][1];

            // Verify required
            if (false !== strpos($rule, 'required')) {
                if (is_null($field) || strlen($field) == 0) {
                    return 'Required';
                } else {
                    $rule = preg_replace('/required,?/i', '', $rule);
                }
            }

            // Search for the RegEx
            if (preg_match_all('/regex:\s?(([^\e]{1})([^\2]*?)\2([imxu]*)){1}/i', $rule, $regs, PREG_PATTERN_ORDER)) {

                // Get the regex(es)
                foreach ($regs[1] as $regex) {
                    // Validate the regex
                    if (!preg_match($regex, $field)) {
                        return 'RegEx ' . $regex;
                    }
                }

                // Remove this rule
                $rule = preg_replace('/regex:\s?(([^\e]{1})([^\2]*?)\2([imxu]*)){1},?/i', '', $rule);
            }

            // Break the other rules
            if (strlen($rule) > 0) {
                // Break the rules
                if (false !== strpos($rule, ',')) {
                    $rule = explode(',', $rule);
                } else {
                    $rule = array($rule);
                }

                // Parse each rule
                foreach ($rule as $r) {
                    // Set the length
                    if (0 === strpos($r, 'length:')) {
                        $r = substr($r, strpos($r, ':') + 1);
                        // Interval
                        if (false !== strpos($r, '-')) {
                            $exp = explode('-', $r);
                            if (strlen($field) < intval($exp[0]) || strlen($field) > intval($exp[1])) {
                                return 'Length out of [' . $exp[0] . ',' . $exp[1] . ']';
                            }
                        }
                        // Minimum
                        elseif (false !== strpos($r, '+')) {
                            if (strlen($field) < intval(str_replace('+', '', $r))) {
                                return 'Length < ' . str_replace('+', '', $r);
                            }
                        }
                        // Maximum
                        else {
                            if (strlen($field) > intval($r)) {
                                return 'Length > ' . $r;
                            }
                        }
                    }

                    // Verify number
                    if (false !== strpos($r, 'number') && !is_numeric($field)) {
                        return 'NaN';
                    }

                    // Verify url
                    if (false !== strpos($r, 'url') && !preg_match('%^http(s)?://[a-z0-9-_.]+\.[a-z]{2,4}%i', $field)) {
                        return 'Not a valid URL.';
                    }

                    // Verify email
                    if (false !== strpos($r, 'email') && !preg_match('/\A(?:^[a-z0-9,!#$%&\'*+\/=?\^_`{|}~-]+(\.[a-z0-9,!#$%&\'*+\/=?\^_`{|}~-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*\.([a-z]{2,})$)\Z/i', $field)) {
                        return 'Not a valid e-mail address.';
                    }

                    // Verify confirmation field
                    if (false !== strpos($r, 'confirm')) {
                        // Get the field
                        if (strlen($r = substr($r, strpos($r, ':') + 1)) > 0 && $input->post($r) !== $field) {
                            return 'Does not match field "' . $r . '".';
                        }
                    }
                }
            }

            // All went well
            return true;
        }

        // There was no session started
        if (session_id() == '') {
            $session = octoms('session')->start();
        } else {
            $session = octoms('session');
        }

        // Clear all previous errors
        $session->flash_get('_form_errors_');

        // Form rules were applied
        if (null !== $rules = $session->flash_get('_form_rules_')) {
            // All clear so far
            $result = true;
            $errors = array();

            // Process each field
            foreach ($rules as $field => $rule) {
                // If the rule was not passed
                if (true !== $v = validate($field, $rule)) {
                    // Save this error
                    $errors[$field] = array($rule[1], $v);

                    // The result is no longer true
                    $result = false;
                }
            }

            // Save the errors, if there were any
            if (!$result) {
                $session->flash_set('_form_errors_', $errors);
                return $errors;
            } else {
                return true;
            }
        } else {
            // No rules were set so everything is ok
            return true;
        }
    }
}
    
/* End Of File <html.inc> */