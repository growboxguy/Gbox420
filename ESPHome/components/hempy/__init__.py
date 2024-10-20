import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, number, switch
from esphome.const import CONF_ID

# Define a namespace for the component
hempy_ns = cg.esphome_ns.namespace('hempy')
HempyBucket = hempy_ns.class_('HempyBucket', cg.PollingComponent)

# Configuration schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HempyBucket),
    cv.Required('weight_sensor'): cv.use_id(sensor.Sensor),
    cv.Required('start_watering_weight'): cv.use_id(number.Number),
    cv.Required('watering_increments'): cv.use_id(number.Number),
    cv.Required('max_watering_weight'): cv.use_id(number.Number),
    cv.Required('max_watering_time'): cv.use_id(number.Number),
    cv.Required('drain_wait_time'): cv.use_id(number.Number),
    cv.Required('drain_target_weight'): cv.use_id(number.Number),
    cv.Required('waterpump'): cv.use_id(switch.Switch),                   # Reference the water pump switch    
}).extend(cv.polling_component_schema(default_update_interval="1s"))

# Code generation when configuring the component
async def to_code(config):
    weight_sensor = await cg.get_variable(config['weight_sensor'])
    start_watering_weight = await cg.get_variable(config['start_watering_weight'])
    watering_increments = await cg.get_variable(config['watering_increments'])
    max_watering_weight = await cg.get_variable(config['max_watering_weight'])
    max_watering_time = await cg.get_variable(config['max_watering_time'])
    drain_wait_time = await cg.get_variable(config['drain_wait_time'])
    drain_target_weight = await cg.get_variable(config['drain_target_weight']) 
    waterpump = await cg.get_variable(config['waterpump'])

    var = cg.new_Pvariable(config[CONF_ID], weight_sensor, start_watering_weight, watering_increments, max_watering_weight, max_watering_time, drain_wait_time, drain_target_weight, waterpump)
    await cg.register_component(var, config)