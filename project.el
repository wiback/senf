;; Configuration file for cc-ide.el (Emacs C++ IDE extension, see http://g0dil.de)

 (defconst senf-c-style
  '((c-basic-offset              . 4)
    (c-cleanup-list             . (empty-defun-braces 
                                   defun-close-semi 
                                   list-close-comma 
                                   scope-operator 
                                   compact-empty-funcall))
    (c-hanging-braces-alist      . ((namespace-open after)
                                    (namespace-close before after)
                                    (brace-list-open)
                                    (brace-entry-open)
                                    (substatement-open after)
                                    (block-close . c-snug-do-while)
                                    (extern-lang-open after)
                                    (inexpr-class-open after)
                                    (inexpr-class-close before)))
    (c-offsets-alist             . ((namespace-open . 0)
                                    (namespace-close . -)
                                    (innamespace . +)
                                    (statement-block-intro . +)
                                    (substatement-open . 0)
                                    (label . 0)
                                    (statement-cont . +))) ))

(c-add-style "senf" senf-c-style)

(set (make-local-variable 'ccide-file-vars)
     '(( fill-column  . 100    )
       ( c-file-style . "senf" )
       ( indent-tabs-mode . nil )
       ( ispell-local-dictionary . "american" )))

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

(let ((local-conf (expand-file-name "project-local.el" ccide-project-root)))
  (if (file-readable-p local-conf)
      (load-file local-conf)))
