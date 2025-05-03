#!/bin/bash

if [[ $2 == tests/* ]]; then
  C="../components"
else
  C="components"
fi

esphome -s external_components_source $C ${1:-run} esp32-example.yaml
