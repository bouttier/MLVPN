# MLVPN
# (c) 2015 Laurent Coustet <ed@zehome.com>

SUBDIRS = systemd
if INSTALL_EXAMPLES
dist_doc_DATA = mlvpn.conf \
    mlvpn_updown.sh mlvpn.init.d mlvpn.default \
    mlvpn_updown_openbsd.sh
endif
# Force generation of the configuration files for debian package.
nodist_doc_DATA = mlvpn.conf mlvpn_updown.sh

mlvpn.conf: mlvpn.conf.in
mlvpn_updown.sh: mlvpn_updown.sh.in
mlvpn.init.d: mlvpn.init.d.in

TEMPLATES   = mlvpn.conf mlvpn_updown.sh mlvpn.init.d
EXTRA_DIST  = mlvpn.conf.in mlvpn_updown.sh.in mlvpn.init.d.in
CLEANFILES  = $(TEMPLATES)

include $(top_srcdir)/template.am
