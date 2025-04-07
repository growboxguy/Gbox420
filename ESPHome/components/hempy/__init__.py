import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, number, switch, text_sensor
from esphome.const import CONF_ID

# Define a namespace for the component
hempy_ns = cg.esphome_ns.namespace('hempy')
HempyBucket = hempy_ns.class_('HempyBucket', cg.PollingComponent)

# Schema for a single HempyBucket instance
HEMPY_BUCKET_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(HempyBucket),
    cv.Required('name'): cv.string,
    cv.Required('state_sensor'): cv.use_id(text_sensor.TextSensor),
    cv.Required('weight_sensor'): cv.use_id(sensor.Sensor),
    cv.Required('start_watering_weight'): cv.use_id(number.Number),
    cv.Required('watering_increments'): cv.use_id(number.Number),
    cv.Required('max_watering_weight'): cv.use_id(number.Number),
    cv.Required('manual_watering_time'): cv.use_id(number.Number),
    cv.Required('pump_timeout'): cv.use_id(number.Number),
    cv.Required('drain_wait_time'): cv.use_id(number.Number),
    cv.Required('drain_target_weight'): cv.use_id(number.Number),
    cv.Required('evaporation_target_weight'): cv.use_id(number.Number),
    cv.Required('dry_weight'): cv.use_id(sensor.Sensor),
    cv.Required('wet_weight'): cv.use_id(sensor.Sensor),
    cv.Required('waterpump'): cv.use_id(switch.Switch),
    cv.Optional('update_interval', default="30s"): cv.update_interval,
})

# Configuration schema for the list of HempyBuckets
CONFIG_SCHEMA = cv.Schema({
    cv.Optional('buckets'): cv.ensure_list(HEMPY_BUCKET_SCHEMA)
})

# Code generation for handling multiple HempyBucket instances
async def to_code(config):
    for bucket_conf in config['buckets']:
        name = bucket_conf['name']
        state_sensor = await cg.get_variable(bucket_conf['state_sensor'])
        weight_sensor = await cg.get_variable(bucket_conf['weight_sensor'])
        start_watering_weight = await cg.get_variable(bucket_conf['start_watering_weight'])
        watering_increments = await cg.get_variable(bucket_conf['watering_increments'])
        max_watering_weight = await cg.get_variable(bucket_conf['max_watering_weight'])
        manual_watering_time = await cg.get_variable(bucket_conf['manual_watering_time'])        
        pump_timeout = await cg.get_variable(bucket_conf['pump_timeout'])
        drain_wait_time = await cg.get_variable(bucket_conf['drain_wait_time'])
        drain_target_weight = await cg.get_variable(bucket_conf['drain_target_weight'])
        evaporation_target_weight = await cg.get_variable(bucket_conf['evaporation_target_weight'])
        dry_weight = await cg.get_variable(bucket_conf['dry_weight'])
        wet_weight = await cg.get_variable(bucket_conf['wet_weight'])
        waterpump = await cg.get_variable(bucket_conf['waterpump'])
        update_interval = bucket_conf['update_interval']

        # Create the HempyBucket instance with all required parameters
        var = cg.new_Pvariable(bucket_conf[CONF_ID], name, state_sensor, weight_sensor, start_watering_weight, watering_increments, max_watering_weight, manual_watering_time, pump_timeout, drain_wait_time, drain_target_weight, evaporation_target_weight, dry_weight, wet_weight, waterpump, cg.uint32(update_interval))
        await cg.register_component(var, bucket_conf)