;; Configuration file for cc-ide.el (Emacs C++ IDE extension, see http://g0dil.de)

(defun check-namespace-indent (arg)
  (save-excursion
    (back-to-indentation)
    (if (and (looking-at "namespace")
             (looking-at ".*{")
             (not (looking-at ".*}")))
        [0] '+)))

 (defconst senf-c-style
  '((c-basic-offset              . 4)
    (c-backslash-column          . 98)
    (c-cleanup-list              . (empty-defun-braces
                                    defun-close-semi
                                    list-close-comma
                                    scope-operator))
    (c-hanging-braces-alist      . ((namespace-open after)
                                    (namespace-close before after)
                                    (brace-list-open)
                                    (brace-entry-open)
                                    (substatement-open after)
                                    (block-close . c-snug-do-while)
                                    (extern-lang-open after)
                                    (inexpr-class-open after)
                                    (inexpr-class-close before)))
    (c-offsets-alist             . ((namespace-open . [0])
                                    (namespace-close . [0])
                                    (innamespace . check-namespace-indent)
                                    (statement-block-intro . +)
                                    (substatement-open . 0)
                                    (label . 0)
                                    (statement-cont . +))) ))

(c-add-style "senf" senf-c-style)

(set (make-local-variable 'ccide-file-vars)
     '((fill-column  . 100)
       (comment-column . 40)
       (c-file-style . "senf")
       (indent-tabs-mode . nil)
       (ispell-local-dictionary . "american")
       (compile-command . "scons -u test")))

(set (make-local-variable 'ccide-default-copyright)
     (concat "//\n"
             "// This program is free software; you can redistribute it and/or modify\n"
             "// it under the terms of the GNU General Public License as published by\n"
             "// the Free Software Foundation; either version 2 of the License, or\n"
             "// (at your option) any later version.\n"
             "//\n"
             "// This program is distributed in the hope that it will be useful,\n"
             "// but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
             "// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
             "// GNU General Public License for more details.\n"
             "//\n"
             "// You should have received a copy of the GNU General Public License\n"
             "// along with this program; if not, write to the\n"
             "// Free Software Foundation, Inc.,\n"
             "// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.\n"))

(set (make-local-variable 'ccide-project-name) "SENF")

(let ((local-conf (ccide-project-search-upwards "project-local.el")))
  (if local-conf
      (load-file local-conf)))

(defun flyspell-cc-progmode-verify ()
  "Replacement for standard flyspell-generic-progmode-verify which
checks for C/C++ preproc directives. Additionally, anything after ^L
is ignored (Those are the file local variables and local words)."
  (let ((f (get-text-property (point) 'face)))
    (and (memq f flyspell-prog-text-faces)
         (not (save-excursion
                (beginning-of-line)
                (looking-at "\\(//\\)?#")))
         (not (let ((l (max (point-min) (- (point-max) 4096))))
                (and (< l (point))
                     (save-excursion (search-backward "" l t))))))))

(defun flyspell-cc-mode ()
  "Torn on `flyspell-mode` for comments and strings in C/C++ mode."
  (interactive)
  (setq flyspell-generic-check-word-p 'flyspell-cc-progmode-verify)
  (flyspell-mode 1))

;; Better set this here than in the file variables since the setting
;; is only valid if project.el is loaded ...
(set (make-local-variable 'ispell-personal-dictionary)
     (expand-file-name "tools/senf.dict" ccide-project-root))
(flyspell-cc-mode)

(defun senf-new-file-hook ()
  (when (string-match "\\.test\\.cc$" (buffer-file-name))
    (save-excursion
      (goto-char (point-min))
      (search-forward "auto_unit_test.hpp")
      (beginning-of-line)
      (delete-region (point) (progn (end-of-line) (point)))
      (insert "#include <senf/Utils/auto_unit_test.hh>"))))

(add-hook 'ccide-new-file-hooks 'senf-new-file-hook nil t)

(setq indent-tabs-mode nil) ;; needed since whitespace-mode caches this value ...
(whitespace-mode 1)


;; Local Variables:
;; indent-tabs-mode: nil
;; End:
