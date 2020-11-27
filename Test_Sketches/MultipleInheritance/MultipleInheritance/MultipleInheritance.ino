

class Common
{
public:
  void Shout()
  {
    Serial.println("Hey!");
  }
};

class Dht : public Common
{
};

class Fan : virtual public Common
{
};

class Common2 : virtual public Common
{
public:
  void Shout2()
  {
    Serial.println("Hi Buddy!");
  }
};

class Bar : public Common2, public Fan
{
};

Common *a;
Dht *b;
Fan *c;
Bar *d;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  a = new Common();
  b = new Dht();
  c = new Fan();
  d = new Bar();
}

void loop()
{
  // put your main code here, to run repeatedly:
  d->Shout();
  d->Shout2();
  a->Shout();
  c->Shout();
  delay(1000);
}
