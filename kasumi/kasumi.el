(defvar kasumi-mode-hook nil)
(defvar kasumi-mode-map
  (let ((map (make-keymap)))
    (define-key map "\C-\\" 'kasumi-send-load) ;; inspect?
    map)
  "Keymap for Kasumi Major mode")

;; getline needs newline("\n")
(defun buffer-string-no-properties ()
  (buffer-substring-no-properties (point-min) (point-max)))

(defun count-line-string (s)
  (if (= (length s) 0)
      0
    (+ 1 (count-if (lambda (x) (= x ?\n)) s))
    ))

(defun take-nth-sub (lis n one)
  (if (= n 0)
      (cons (reverse one) lis)
    (take-nth-sub (cdr lis) (- n 1)
                  (cons (car lis) one))))

;; [a] -> ([a] . [a])
(defun take-nth (l n)
  (take-nth-sub l n '()))
(defun string-join (lis sep)
  (mapconcat 'identity lis sep))

(defcustom kasumi-where-is-shiranui nil
  "Path where shiranui locates.")

;; http://www.gnu.org/software/emacs/manual/html_node/elisp/Asynchronous-Processes.html#Asynchronous-Processes
(defun kasumi-start-shiranui (program)
  (let ((process-connection-type nil)) ;; start in pipe
    ;; start-process needs absolute path?
    (start-process "shiranui" "shiranui" (file-truename program) "--server")
    ))

(defconst kasumi-font-lock-keywords
  (list
   '("let[ \t\n]*[a-zA-Z]+[0-9]*" 0 font-lock-function-name-face)
   '("mut[ \t\n]*[a-zA-Z]+[0-9]*" 0 font-lock-variable-name-face)
   ;; overwrite for let,mut.
   '( "\\<\\(else\\|if\\|let\\|mut\\|then\\|return\\)\\>" 0 font-lock-keyword-face t)
   '("\\(\\\\\\)" 0 font-lock-constant-face)
   '("\\<\\(and\\|or\\)\\>" 0 font-lock-builtin-face)
   '("not\\>" 0 font-lock-builtin-face)
   '("\\<[\\-+]*[0-9]*\\.?[0-9]+\\>" 0 font-lock-constant-face)
   ))

(defconst kasumi-command-load
  "load")

(defun kasumi-send-command (command value)
  (process-send-string shiranui-process
   (concat (number-to-string (count-line-string value)) " " command "\n" value)
   ))

;; string -> (command value rest)
(defun kasumi-parse-sub (str)
  (let* ((lines (split-string str "\n"))
         (first-line (split-string (car lines) " "))
         (command-line-length (string-to-number (car first-line)))
         (command (string-join (cdr first-line) " "))
         (value-and-rest (take-nth (cdr lines) command-line-length)))
    (list command (string-join (car value-and-rest) "\n")
                  (string-join (cdr value-and-rest) "\n"))))

;; string -> [(command . value)]
(defun kasumi-parse (str)
  (if (= (length str) 0)
      '()
    (let ((command-value-rest (kasumi-parse-sub str)))
      (cons (cons (car command-value-rest) (cadr command-value-rest))
            (kasumi-parse (caddr command-value-rest))))))


;; need parsing?
(defun kasumi-process-filter (process str)
  (let ((pairs-command-value (kasumi-parse str)))))

(defun kasumi-send-load ()
  (interactive)
  (kasumi-send-command kasumi-command-load (buffer-string-no-properties)))

;; http://www.emacswiki.org/emacs/ModeTutorial
(defun kasumi-mode ()
  "Major mode for editing Shiranui files"
  (interactive)
  (kill-all-local-variables)
  (use-local-map kasumi-mode-map)
  (set (make-local-variable 'font-lock-defaults) '(kasumi-font-lock-keywords))

  (make-local-variable 'receive-in-progress)
  (make-local-variable 'receiving-str)
  (set (make-local-variable 'shiranui-process)
       (if (null kasumi-where-is-shiranui)
           (kasumi-start-shiranui (read-file-name "Shiranui Path:"))
           (kasumi-start-shiranui kasumi-where-is-shiranui)))
  (set-process-filter shiranui-process 'kasumi-process-filter)
  ;; (set-syntax-table kasumi-mode-syntax-table)
  (setq major-mode 'kasumi-mode)
  (setq mode-name "Kasumi")
  (run-hooks 'kasumi-mode-hook))

(provide 'kasumi-mode)
