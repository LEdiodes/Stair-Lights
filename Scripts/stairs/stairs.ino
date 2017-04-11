// Fade light each step strip

void colourWipeDown(uint32_t c, uint16_t wait) {

for (uint16_t j = 0; j < 15; j++){

int start = strip.numPixels()/15 *j;

Serial.println(j);

for (uint16_t i = start; i < start + 35; i++){

strip.setPixelColor(i, c);

}

strip.show();

delay(wait);

}

}
