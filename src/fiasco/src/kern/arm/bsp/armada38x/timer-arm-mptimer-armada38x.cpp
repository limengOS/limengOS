// --------------------------------------------------------------------------
IMPLEMENTATION[arm && pf_armada38x && mptimer]:

PRIVATE static Mword Timer::interval()
{
  return 533333;
}
