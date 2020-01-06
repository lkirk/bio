VE := ve
PIP := $(VE)/bin/pip
PYTHON := $(VE)/bin/python
PRE_REQS := pre_requirements.txt
REQS_TXT := requirements.txt
DEV_REQS := dev_requirements.txt
SCRIPTS := scripts

# written in reverse to have install
# as the default target. more portable
# than using an extension for this.
# keep this at the top
install: _install-requirements
	 $(PIP) install -e .

# Create a development environment
dev: install
	$(PIP) install -r $(DEV_REQS)
	$(VE)/bin/jupyter labextension install jupyterlab_vim

_install-requirements: $(VE)
	$(PIP) install -r $(PRE_REQS)
	$(SCRIPTS)/install-pysam
	$(PIP) install -r $(REQS_TXT)

clean:
	rm -rf $(VE)

$(VE):
	python -m venv $@
	$(VE)/bin/pip install -U pip setuptools
