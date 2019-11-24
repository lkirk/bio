VE := ve
PIP := $(VE)/bin/pip
PYTHON := $(VE)/bin/python

build: $(VE)
	$(PYTHON) setup.py build_ext --inplace

test: build
	$(VE)/bin/py.test -vv --doctest-cython

$(VE):
	python -m venv $@
	$(VE)/bin/pip install -U pip setuptools

install-requirements: $(VE)
	$(PIP) install Cython

install-test-requirements: install-requirements
	$(PIP) install pytest pytest-cython

install: install-requirements
	 $(PIP) install -e .

DEVEL-TOOLS := IPython ipdb
setup-devel: install install-test-requirements
	$(PIP) install $(DEVEL-TOOLS)

clean:
	-[ -e $(PYTHON) ] && $(PYTHON) setup.py clean
	rm -rf $(VE) bio.egg-info build
	rm -r $$(find . -name __pycache__ -delete) || true
	rm -f $$(find . -name '*.pyx' | sed -re's/.pyx$$/.c/') || true
	rm -f $$(find . -name '*.pyx' | sed -re's/.pyx$$/.so/') || true
