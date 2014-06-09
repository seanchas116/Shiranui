(defvar kasumi-mode-hook nil)
(defvar kasumi-mode-map
  (let ((map (make-keymap)))
    (define-key map "\C-\\" nil) ;; inspect?
    map)
  "Keymap for Kasumi Major mode")

;; http://www.gnu.org/software/emacs/manual/html_node/elisp/Asynchronous-Processes.html#Asynchronous-Processes
(defun start-shiranui (program)
  (let ((process-connection-type nil)) ;; start in pipe
    ;; start-process needs absolute path?
    (start-process "shiranui" "shiranui" (file-truename program))
    ))

;; getline needs newline("\n")
(defun buffer-string-no-properties ()
  (buffer-substring-no-properties (point-min) (point-max)))


(defconst kasumi-font-lock-keywords
  (list
   (cons "\\<\\(else\\|if\\|let\\|mut\\|then\\|return\\)\\>" 'font-lock-keyword-face)
   (cons "\\(\\\\\\)" 'font-lock-constant-face)
   (cons "\\<\\(and\\|or\\)\\>" 'font-lock-builtin-face)
   (cons "not\\>" 'font-lock-builtin-face)
   ;; (cons "let[ \t\n]*[a-zA-Z]+[0-9]*" 'font-lock-variable-name-face)
   (cons "\\<[\\-+]*[0-9]*\\.?[0-9]+\\>" font-lock-constant-face)
   ))

;; http://www.emacswiki.org/emacs/ModeTutorial
(defun kasumi-mode ()
  "Major mode for editing Shiranui files"
  (interactive)
  (kill-all-local-variables)
  (make-local-variable 'shiranui-process)
  (set (make-local-variable 'font-lock-defaults) '(kasumi-font-lock-keywords))
  ;; (set-syntax-table kasumi-mode-syntax-table)
  (use-local-map kasumi-mode-map)
  (setq major-mode 'kasumi-mode)
  (setq mode-name "Kasumi")
  (run-hooks 'kasumi-mode-hook))

(provide 'kasumi-mode)