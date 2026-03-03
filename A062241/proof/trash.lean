import Mathlib.NumberTheory.LegendreSymbol.JacobiSymbol

open Int
open NumberTheorySymbols

def isSmooth (a k : ℕ) : Prop :=
∀ p : ℕ, Nat.Prime p → p ∣ a → p ≤ k

def isSumOfTwoSmooth (m k : ℕ) : Prop :=
∃ a b : ℕ, m = a + b ∧ isSmooth a k ∧ isSmooth b k ∧ a > 0 ∧ b > 0

def isPseudosquare (a : ℤ) (p : ℕ) : Prop :=
Nat.Prime p ∧ p ≠ 2 ∧ a ≡ 1 [ZMOD 8] ∧ J(a|p) = 1

lemma mod_4_to_even (A : ℕ) (h : (2 * A) % 4 = 0) : Even A := by
  have : (2 * 2) ∣ (2 * A)  := Nat.dvd_of_mod_eq_zero h
  have := Nat.dvd_of_mul_dvd_mul_left (by norm_num) this
  exact even_iff_two_dvd.mpr this

lemma jacobi_add_left (a c : ℤ) (p : ℕ) (h_div : (p : ℤ) ∣ c) :
  J(a + c| p) = J(a|p) := by
  nth_rewrite 1 [jacobiSym.mod_left]
  nth_rewrite 2 [jacobiSym.mod_left]
  rcases exists_eq_mul_right_of_dvd h_div with ⟨c, h_eq⟩
  rw [h_eq]
  simp

lemma jacobi_eq_prod_factorization (a : ℤ) (b : ℕ) (hb_ne0 : b ≠ 0) (hb_odd : Odd b) :
  J(a|b) = b.factorization.prod (fun p k => J(a|p) ^ k) := by
  induction b using induction_on_primes with
  | zero => simp
  | one => simp
  | prime_mul p q h_prime h_ind =>
    have hp_ne0 : p ≠ 0 := left_ne_zero_of_mul hb_ne0
    have hq_ne0 : q ≠ 0 := right_ne_zero_of_mul hb_ne0
    have : NeZero (p) := of_not_not ((not_iff_not.mpr not_neZero).mpr hp_ne0)
    have : NeZero q := of_not_not ((not_iff_not.mpr not_neZero).mpr hq_ne0)
    rw [jacobiSym.mul_right, Nat.factorization_mul, Finsupp.prod_add_index, <- h_ind]
    · rw [Nat.Prime.factorization h_prime]
      simp
    · exact hq_ne0
    · exact (Nat.odd_mul.mp hb_odd).right
    · intros
      simp
    · intro p hp_in b1 b2
      apply pow_add
    · exact hp_ne0
    · exact hq_ne0

lemma jacobi_2_pow_2_is_1 (B : ℕ) (hB : Odd B) : J(2|B)^2 = 1 := by
  rw [jacobiSym.at_two hB]
  have h_lt : (B % 8) < 8 := Nat.mod_lt B (by norm_num)
  interval_cases bmod : (B % 8)
  all_goals -- even case
    try have mod2 : B % 2 = 0 := by omega
    try have := Nat.odd_iff.mp hB
    try rw [this] at mod2
    try contradiction
  all_goals -- odd case
    have := ZMod.χ₈_nat_mod_eight B
    rw [bmod] at this
    rw [this]
    simp

lemma neg_mul_minus_1 (a : ℕ) : -(a:ℤ) = -1 * a := by simp

lemma odd_minus_1_over_2 (A : ℕ) (ha_odd : Odd A) :
  A/2 = (A-1)/2 := by
  obtain ⟨k, rfl⟩ := ha_odd
  rw [<- Nat.div2_val]
  rw [Nat.div2_bit1 k]
  simp

lemma jacobi_reciprocity (A B : ℕ) (ha_odd : Odd A) (hb_odd : Odd B) (hab_coprime : B.gcd A = 1)
  : J(A|B) * J(B|A) = (-1)^(((A-1)/2) * ((B-1)/2)) := by
  rw [jacobiSym.quadratic_reciprocity ha_odd hb_odd]
  rw [Int.mul_assoc, <- pow_two, jacobiSym.sq_one]
  · rw [odd_minus_1_over_2 A ha_odd]
    rw [odd_minus_1_over_2 B hb_odd]
    ring
  · exact hab_coprime

lemma exists_prime_not_pseudosquare (m a b pn : ℕ)
(h_pn : Nat.Prime pn ∧ pn ≠ 2) (h_sum : isSumOfTwoSmooth m pn) :
∃ pi : ℕ, Nat.Prime pi ∧ pi ≠ 2 ∧ pi ≤ pn ∧
¬ isPseudosquare (-(m : ℤ)) pi := by
--
rcases h_sum with ⟨a, b, hm_ab, ha_smooth, hb_smooth, ha_pos, hb_pos⟩
--
by_contra h_contra
simp only [ne_eq, not_exists, not_and, not_not] at h_contra
--
have h_contra_pn := h_contra pn h_pn.left h_pn.right (le_refl pn)
unfold isPseudosquare at h_contra_pn
have h_mod8 := Int.ModEq.neg h_contra_pn.right.right.left
simp only [neg_neg, reduceNeg] at h_mod8
--
have hm_odd : Odd m := by
  have : m ≡ 1 [ZMOD 2] := by
    have h_2div8 : (2: ℤ) ∣ 8 := by decide
    have h_mod2' := Int.ModEq.of_dvd h_2div8 h_mod8
    exact h_mod2'
  unfold ModEq at this
  simp only [one_emod_two] at this
  norm_cast at this
  exact Nat.odd_iff.mpr this
--
have h_parity : Odd a ↔ Even b := by
  apply Nat.odd_add.mp
  rw [<- hm_ab]
  exact hm_odd
--
wlog ha_even: Even a
{ have hm_ba : m = b + a := by
    rw [add_comm]
    exact hm_ab
  have h_inv_parity : Odd b ↔ Even a := by
    apply not_iff_not.mp
    simp [h_parity]
  have hb_even : Even b := by
    simp only [Nat.not_even_iff_odd] at ha_even
    exact h_parity.mp ha_even
  exact this m b a pn h_pn b a hm_ba hb_smooth ha_smooth hb_pos ha_pos
    h_contra h_contra_pn h_mod8 hm_odd h_inv_parity hb_even
}
--
have hb_odd : Odd b := by
  have := (not_iff_not.mpr h_parity).mp
  simp only [Nat.not_odd_iff_even, Nat.not_even_iff_odd] at this
  exact this ha_even
--
rcases Nat.exists_eq_two_pow_mul_odd (pos_iff_ne_zero.mp ha_pos)
  with ⟨k, A, hA_odd, h_eq⟩
--
let B := b
have hA_ne0 : A ≠ 0 := by
  rw [h_eq] at ha_pos
  simp only [gt_iff_lt, Nat.ofNat_pos, pow_pos, mul_pos_iff_of_pos_left] at ha_pos
  exact pos_iff_ne_zero.mp ha_pos
--
have hab_coprime : B.gcd A = 1 := by
  by_contra h_gcd
  obtain ⟨p, hp_prime, hp_dvd⟩ := Nat.exists_prime_and_dvd h_gcd
  have := Odd.of_dvd_nat hb_odd (Nat.gcd_dvd B A).left
  have hp_odd := Odd.of_dvd_nat this hp_dvd
  have hp_not2: p ≠ 2 := by
    by_contra
    rw [this] at hp_odd
    contradiction
  have hp_not0: p ≠ 0 := by
    by_contra
    rw [this] at hp_odd
    contradiction
  have hp_div_B := dvd_trans hp_dvd (Nat.gcd_dvd B A).left
  have hp_div_A := dvd_trans hp_dvd (Nat.gcd_dvd B A).right
  have hp_div_a : p ∣ a := by
    rw [h_eq]
    exact dvd_mul_of_dvd_right hp_div_A (2^k)
  have hp_div_m : p ∣ m := by
    rw [hm_ab]
    exact Dvd.dvd.add hp_div_a hp_div_B
  have hp_div_negm :  (p : ℤ) ∣ (-(m : ℤ)) := by
    rw [neg_mul_minus_1]
    apply Int.dvd_mul_of_dvd_right
    norm_cast
  have hp_lt_pn : p ≤ pn := by
    exact hb_smooth p hp_prime hp_div_B
  have h_pseudo := (h_contra p hp_prime hp_not2 hp_lt_pn).right.right.right
--
  have hp_gt0 := Nat.Prime.pos hp_prime
  have : p ≠ 0 ∧ (-(m : ℤ)).gcd p ≠ 1 := by
    constructor
    · exact hp_not0
    · have : (0 : ℤ) < p := by norm_cast
      have := Int.gcd_eq_right (Int.le_of_lt this) hp_div_negm
      norm_cast at this
      rw [this]
      exact hp_prime.ne_one
  rw [jacobiSym.eq_zero_iff.mpr this] at h_pseudo
  contradiction
--
have h_prime_div_A : ∀p, Nat.Prime p → p ∣ A → p ≤ pn ∧ p ≠ 2 := by
  intros p hp p_div_A
  have hp_div_a : p ∣ a := by
    rw [h_eq]
    exact (dvd_mul_of_dvd_right p_div_A (2^k))
  constructor
  · exact ha_smooth p hp hp_div_a
  · exact Odd.ne_two_of_dvd_nat hA_odd p_div_A
--
have h_Jmp_ofA : ∀p, Nat.Prime p → p ∣ A → J(-m|p) = 1 := by
  intro p hp hp_div
  have h := h_prime_div_A p hp hp_div
  have h_contra_p := h_contra p hp h.right h.left
  unfold isPseudosquare at h_contra_p
  exact h_contra_p.right.right.right
--
have h_JBp_ofA : ∀p, Nat.Prime p → p ∣ A → J(-b|p) = 1 := by
  intro p hp hp_div
  have := h_Jmp_ofA p hp hp_div
  rw [hm_ab] at this
  rw [<- this]
  rw [<- jacobi_add_left (-b) (-a) p]
  · simp only [Nat.cast_add, neg_add_rev]
  · apply dvd_neg.mpr
    norm_cast
    rw[h_eq]
    exact (dvd_mul_of_dvd_right hp_div (2^k))
--
have h_prime_div_B : ∀p, Nat.Prime p → p ∣ B → p ≤ pn ∧ p ≠ 2 := by
  intros p hp p_div_B
  constructor
  · exact hb_smooth p hp p_div_B
  · exact Odd.ne_two_of_dvd_nat hb_odd p_div_B
--
have h_Jmp_ofB : ∀p, Nat.Prime p → p ∣ B → J(-m|p) = 1 := by
  intro p hp hp_div
  have h := h_prime_div_B p hp hp_div
  have h_contra_p := h_contra p hp h.right h.left
  unfold isPseudosquare at h_contra_p
  exact h_contra_p.right.right.right
--
have h_JBA : J(-b|A) = 1 := by
  rw [jacobi_eq_prod_factorization (-b) A hA_ne0 hA_odd]
  let f := fun p k => J(-b|p)^k
  rw [Nat.prod_factorization_eq_prod_primeFactors f]
  apply Finset.prod_eq_one
  intro p hp_in
  have := (Nat.mem_primeFactors_of_ne_zero hA_ne0).mp hp_in
  unfold f
  have h_jmp := h_Jmp_ofA p this.left this.right
  rw [hm_ab] at h_jmp
  rw [<- jacobi_add_left (-b) (-a) p]
  · simp only [Nat.cast_add, neg_add_rev] at h_jmp
    rw [h_jmp]
    simp
  · have hp_div_a : (p : ℤ) ∣ (-a) := by
      rw [h_eq]
      have := (dvd_mul_of_dvd_right this.right (2^k))
      apply dvd_neg.mpr
      norm_cast
    exact hp_div_a
--
have h_JAB: J(-a|B) = 1 := by
  have hb_ne0 := ne_of_gt hb_pos
  rw [jacobi_eq_prod_factorization (-a) B hb_ne0 hb_odd]
  let f := fun p k => J(-a|p)^k
  rw [Nat.prod_factorization_eq_prod_primeFactors f]
  apply Finset.prod_eq_one
  intro p hp_in
  have := (Nat.mem_primeFactors_of_ne_zero hb_ne0).mp hp_in
  unfold f
  have h_jmp := h_Jmp_ofB p this.left this.right
  rw [hm_ab] at h_jmp
  rw [<- jacobi_add_left (-a) (-b) p]
  · simp only [Nat.cast_add, neg_add_rev] at h_jmp
    rw [add_comm, h_jmp]
    simp
  · have hp_div_b : (p : ℤ) ∣ (-b) := by
      apply dvd_neg.mpr
      norm_cast
      exact this.right
    exact hp_div_b
--
let E := (A*B-1)/2 + ((A-1)/2) * ((B-1)/2)
have h_product: J(2^k|B) * (-1)^E = 1 := by
  have : J(-1|A * B) * J(2^k|B) * J(A|B) * J(B|A) = 1 := by
    have : (NeZero A) := ⟨ hA_ne0 ⟩
    have : (NeZero B) := ⟨ ne_of_gt hb_pos ⟩
    rw [jacobiSym.mul_right]
    have : J(-a|B) * J(-b|A) = 1 := by
      rw [h_JAB, h_JBA]
      simp
    rw [h_eq] at this
    repeat rw [neg_mul_minus_1, jacobiSym.mul_left] at this
    rw [Int.natCast_mul, jacobiSym.mul_left] at this
    nth_rw 3 [<- this]
    norm_cast
    ring
  nth_rw 3 [Int.mul_comm] at this
  rw [jacobiSym.at_neg_one, ZMod.χ₄_eq_neg_one_pow, Int.mul_assoc] at this
  · rw [jacobi_reciprocity A B hA_odd hb_odd] at this
    · rw [Int.mul_assoc, <- Int.pow_add] at this
      rw [odd_minus_1_over_2 (A*B) (Odd.mul hA_odd hb_odd)] at this
      exact this
    · exact hab_coprime
  · exact Nat.odd_iff.mp (Odd.mul hA_odd hb_odd)
  · exact (Odd.mul hA_odd hb_odd)
-- helper functions
have then_E_pos : (A % 4 = 1) → (B % 4 = 1) → (-1)^E = 1 := by
  intro hA hB
  have hAB_odd := (Odd.mul hA_odd hb_odd)
  have hAB : (A * B) % 4 = 1 := by rw [Nat.mul_mod, hA, hB]
  have h_A := ZMod.neg_one_pow_div_two_of_one_mod_four hA
  have h_B := ZMod.neg_one_pow_div_two_of_one_mod_four hB
  have h_AB := ZMod.neg_one_pow_div_two_of_one_mod_four hAB
  rw [pow_add, pow_mul]
  repeat rw [<- odd_minus_1_over_2]
  · rw [h_AB, h_A]; simp
  · exact hb_odd
  · exact hA_odd
  · exact hAB_odd
have then_E_neg_A : (A % 4 = 3) → (-1)^E = -1 := by
  intro hA
  rw [pow_add, pow_mul]
  have := Nat.odd_mod_four_iff.mp (Nat.odd_iff.mp hb_odd)
  repeat rw [<- odd_minus_1_over_2]
  · rw [ZMod.neg_one_pow_div_two_of_three_mod_four hA]
    cases this with
    | inl hB =>
      rw [ZMod.neg_one_pow_div_two_of_one_mod_four hB]
      have hAB : (A * B) % 4 = 3 := by rw [Nat.mul_mod, hA, hB]
      rw [ZMod.neg_one_pow_div_two_of_three_mod_four hAB]
      simp
    | inr hB =>
      rw [ZMod.neg_one_pow_div_two_of_three_mod_four hB]
      have hAB : (A * B) % 4 = 1 := by rw [Nat.mul_mod, hA, hB]
      rw [ZMod.neg_one_pow_div_two_of_one_mod_four hAB]
      simp
  · exact hb_odd
  · exact hA_odd
  · exact (Odd.mul hA_odd hb_odd)
have then_E_neg_B : (B % 4 = 3) → (-1)^E = -1 := by
  intro hB
  rw [pow_add, pow_mul]
  have := Nat.odd_mod_four_iff.mp (Nat.odd_iff.mp hA_odd)
  repeat rw [<- odd_minus_1_over_2]
  · cases this with
    | inl hA =>
      have hAB : (A * B) % 4 = 3 := by rw [Nat.mul_mod, hA, hB]
      rw [ZMod.neg_one_pow_div_two_of_one_mod_four hA]
      rw [ZMod.neg_one_pow_div_two_of_three_mod_four hAB]
      simp
    | inr hA =>
      have hAB : (A * B) % 4 = 1 := by rw [Nat.mul_mod, hA, hB]
      rw [ZMod.neg_one_pow_div_two_of_one_mod_four hAB]
      rw [ZMod.neg_one_pow_div_two_of_three_mod_four hA]
      rw [ZMod.neg_one_pow_div_two_of_three_mod_four hB]
      simp
  · exact hb_odd
  · exact hA_odd
  · exact (Odd.mul hA_odd hb_odd)
have then_J_one_1 : (B % 8 = 1) → J(2|B) = 1 := by
  rw [jacobiSym.at_two hb_odd]
  intro hB
  have := (ZMod.natCast_eq_natCast_iff B 1 8).mpr hB
  rw [this]
  simp
have then_J_one_7 : (B % 8 = 7) → J(2|B) = 1 := by
  rw [jacobiSym.at_two hb_odd]
  intro hB
  have := (ZMod.natCast_eq_natCast_iff B 7 8).mpr hB
  rw [this]
  simp
have then_J_neg_3 : (B % 8 = 3) → J(2|B) = -1 := by
  rw [jacobiSym.at_two hb_odd]
  intro hB
  have := (ZMod.natCast_eq_natCast_iff B 3 8).mpr hB
  rw [this]
  simp
have then_J_neg_5 : (B % 8 = 5) → J(2|B) = -1 := by
  rw [jacobiSym.at_two hb_odd]
  intro hB
  have := (ZMod.natCast_eq_natCast_iff B 5 8).mpr hB
  rw [this]
  simp
-- the end is near, doing case on k
have contra_k0 : (k = 0) → False := by
  intro h_k
  rw [h_k] at h_eq
  simp only [pow_zero, one_mul] at h_eq
  rw [h_eq] at ha_even
  have := Nat.not_even_iff_odd.mpr hA_odd
  contradiction
--
rw [hm_ab] at h_mod8
rw [Nat.cast_add] at h_mod8
--
have contra_k11 : (k = 1) → (B % 8 = 1)  → False := by
  intro hk hb8
  rw [hk] at h_eq
  rw [hk] at h_product
  have : a % 8 = 6 := by
    have : b ≡ 1 [ZMOD 8] := by exact_mod_cast hb8
    have := Int.ModEq.sub h_mod8 this
    simp at this
    have : (a : ℤ) ≡ 6 [ZMOD 8] := by
      exact this.trans (by norm_num [Int.ModEq])
    exact_mod_cast this
  have hA_mod : A % 8 = 3 ∨ A % 8 = 7 := by
    have : (2 * A) % 8 = 6 := by
      rw [h_eq] at this; exact this
    omega
  have : A % 4 = 3 := by omega
  have hE : (-1)^E = -1 := then_E_neg_A this
  have hJ : J(2|B) = 1 := then_J_one_1 hb8
  simp only [pow_one] at h_product
  rw [hJ] at h_product
  rw [hE] at h_product
  contradiction
have contra_k15 :  (k = 1) → (B % 8 = 5)  → False := by
  intro hk hb8
  rw [hk] at h_eq
  rw [hk] at h_product
  have : a % 8 = 2 := by
    have : b ≡ 5 [ZMOD 8] := by exact_mod_cast hb8
    have := Int.ModEq.sub h_mod8 this
    simp at this
    have : (a : ℤ) ≡ 2 [ZMOD 8] := by
      exact this.trans (by norm_num [Int.ModEq])
    exact_mod_cast this
  have h_A : A % 4 = 1 := by omega
  have h_B : B % 4 = 1 := by omega
  have hE : (-1)^E = 1 := then_E_pos h_A h_B
  have hJ : J(2|B) = -1 := then_J_neg_5 hb8
  simp only [pow_one] at h_product
  rw [hJ] at h_product
  rw [hE] at h_product
  contradiction
have contra_k13 :  (k = 1) → (B % 8 = 3) → False := by
  intro hk hb8
  have h_A : a % 8 = 4 := by
    have : b ≡ 3 [ZMOD 8] := by exact_mod_cast hb8
    have := Int.ModEq.sub h_mod8 this
    simp at this
    have : (a : ℤ) ≡ 4 [ZMOD 8] := by
      exact this.trans (by norm_num [Int.ModEq])
    exact_mod_cast this
  have h_A : a % 4 = 0 := by omega
  rw [h_eq, hk] at h_A
  simp only [pow_one] at h_A
  have := mod_4_to_even A h_A
  have := Nat.not_odd_iff_even.mpr this
  contradiction
have contra_k17 :  (k = 1) → (B % 8 = 7) → False := by
  intro hk hb8
  have h_A : a % 8 = 0 := by
    have : b ≡ 7 [ZMOD 8] := by exact_mod_cast hb8
    have := Int.ModEq.sub h_mod8 this
    simp at this
    have : (a : ℤ) ≡ 0 [ZMOD 8] := by
      exact this.trans (by norm_num [Int.ModEq])
    exact_mod_cast this
  have h_A : a % 4 = 0 := by omega
  rw [h_eq, hk] at h_A
  simp only [pow_one] at h_A
  have := mod_4_to_even A h_A
  have := Nat.not_odd_iff_even.mpr this
  contradiction
--
have contra_k2 :  (k = 2) → False := by
  intro hk
  rw [hk] at h_eq
  have ha_mod8 : a ≡ 4 [ZMOD 8] := by
    obtain ⟨x, hx⟩ := hA_odd
    rw [h_eq, hx]
    norm_cast
    ring_nf
    rw [add_comm, mul_comm]
    exact Nat.ModEq.modulus_mul_add
  have : B ≡ 3 [ZMOD 8] := by
    have := Int.ModEq.sub h_mod8 ha_mod8
    simp only [add_sub_cancel_left, reduceNeg, reduceSub] at this
    exact this
  have : B % 8 = 3 := by exact_mod_cast this
  have h_B : B % 4 = 3 := by omega
  have hE : (-1)^E = -1 := then_E_neg_B h_B
  rw [jacobiSym.pow_left, hk, hE] at h_product
  rw [jacobi_2_pow_2_is_1 B hb_odd] at h_product
  contradiction
--
have contra_k3 : (n : ℕ) → (k = n + 3) → False := by
  intro n hk
  rw [hk, pow_add] at h_eq
  simp only [Nat.reducePow] at h_eq
  have hA : a ≡ 0 [ZMOD 8] := by
    rw [h_eq]
    apply Dvd.dvd.modEq_zero_int
    norm_cast
    rw [mul_assoc, mul_comm, mul_assoc]
    exact dvd_mul_right 8 (A * 2 ^ n)
  have hB : b ≡ 7 [ZMOD 8] := by
    have := Int.ModEq.sub h_mod8 hA
    simp only [add_sub_cancel_left, reduceNeg, reduceSub] at this
    exact this
  have : b % 8 = 7 := by exact_mod_cast hB
  have hJ : J(2|B) = 1 := then_J_one_7 this
  rw [jacobiSym.pow_left, hJ] at h_product
  simp only [one_pow, reduceNeg, one_mul] at h_product
  have : b % 4 = 3 := by omega
  have hE := then_E_neg_B this
  rw [hE] at h_product
  contradiction
-- YAY
match k with
| 0 => contradiction
| 1 =>
  match hb8 : B % 8 with
  | 0 | 2 | 4 | 6 =>
    have : Even B := by
      rw [Nat.even_iff, ← Nat.mod_mod_of_dvd B (by norm_num : 2 ∣ 8)]
      simp [hb8]
    have := Nat.not_odd_iff_even.mpr this
    contradiction
  | 1 => simp at contra_k11; contradiction
  | 5 => simp at contra_k15; contradiction
  | 3 => simp at contra_k13; contradiction
  | 7 => simp at contra_k17; contradiction
  | x+8 =>
    have : 0 < 8 := by simp
    have : B % 8 < 8 := Nat.mod_lt B this
    rw [hb8] at this
    contradiction
| 2 => contradiction
| n + 3 => simp at contra_k3
