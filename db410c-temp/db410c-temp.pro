TEMPLATE = subdirs
SUBDIRS = app binding package
package.depends += app binding
