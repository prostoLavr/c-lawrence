#!/usr/bin/make

ifneq ("$(wildcard .env)","")
    include .env
    export $(shell sed 's/=.*//' .env)
endif

RUN_ARGS := $(wordlist 2,100,$(MAKECMDGOALS))
$(eval $(RUN_ARGS):;@:)

.PHONY: help

help: ## Help
	@grep -E '(^[a-zA-Z0-9_-]+:.*?##.*$$)|(^##)' $(firstword $(MAKEFILE_LIST)) | awk 'BEGIN {FS = ":.*?## "}{printf "\033[32m%-30s\033[0m %s\n", $$1, $$2}'

run:  ## Run main 
	@gcc main.c -o main -ldl -rdynamic && LD_LIBRARY_PATH=$(CURDIR) ./main

module:
	gcc -shared -include mycustomlib.c -o print_module_var.so -fPIC print_module_var.c

